#include "DHCPServer.h"
#include "../Network/Router.h"
#include <QDebug>

DHCPServer::DHCPServer(int asId, const QSharedPointer<Router> &router, QObject *parent)
    : QObject(parent),
    m_asId(asId),
    m_router(router),
    m_nextAvailableId(1),
    m_currentTime(0) {
    if (m_asId == 1) {
        m_ipPrefix = "192.168.100.";
    } else if (m_asId == 2) {
        m_ipPrefix = "192.168.200.";
    } else {
        qWarning() << "Unsupported AS ID:" << m_asId;
    }

    qDebug() << "DHCP Server initialized for AS ID:" << m_asId << "on Router ID:" << router->getId();
}

DHCPServer::~DHCPServer() {}


void DHCPServer::receivePacket(const PacketPtr_t &packet) {
    if (!packet || packet->getType() != PacketType::Control) {
        qWarning() << "DHCP Server received invalid packet.";
        return;
    }

    QString payload = packet->getPayload();
    qDebug() << "DHCP Server processing packet with payload:" << payload;

    if (payload.contains("DHCP_REQUEST")) {
        assignIP(packet);
    }
}

void DHCPServer::assignIP(const PacketPtr_t &packet) {
    int clientId = packet->getPayload().split(":")[1].toInt();

    if(m_asId == 1 && clientId > 16) {
        qDebug() << "Router Not In Our AS(1)";
        return;
    } else if (m_asId == 2 && clientId < 17) {
        qDebug() << "Router Not In Our AS(2)";
        return;
    }

    qDebug() << "Assigning IP to client" << clientId << "in AS" << m_asId;

    for (const auto &lease : m_leases) {
        if (lease.clientId == clientId) {
            qDebug() << "Client" << clientId << "already has an IP:" << lease.ipAddress << ". Re-sending offer.";
            sendOffer(lease);
            return;
        }
    }

    QString ipAddress = m_ipPrefix + QString::number(m_nextAvailableId);
    DHCPLease lease = {ipAddress, clientId, m_currentTime + LEASE_DURATION};
    m_leases.append(lease);
    m_nextAvailableId++;

    qDebug() << "New IP assigned:" << ipAddress << "for client" << clientId;
    sendOffer(lease);
}

void DHCPServer::sendOffer(const DHCPLease &lease) {
    QString payload = QString("DHCP_OFFER:%1:%2").arg(lease.ipAddress).arg(lease.clientId);
    auto offerPacket = QSharedPointer<Packet>::create(PacketType::Control, payload);

    offerPacket->setTTL(10); // Defaut TTL For Offer DHCP Server

    if (m_router) {
        const auto &ports = m_router->getPorts();
        qDebug() << "Sending DHCP offer from Router" << m_router->getId()
                 << "to client" << lease.clientId << "with IP" << lease.ipAddress;
        qDebug() << "Number of ports on Router" << m_router->getId() << ":" << ports.size();

        for (const auto &port : ports) {
            if (port->isConnected()) {
                port->sendPacket(offerPacket);
                qDebug() << "DHCP Server on Router" << m_router->getId()
                         << "broadcasted DHCP offer:" << payload
                         << "via Port" << port->getPortNumber();
            }
        }
    } else {
        qWarning() << "DHCP Server has no associated Router to broadcast offers.";
    }
}

void DHCPServer::tick(int currentTime) {
    m_currentTime = currentTime;
    reclaimExpiredLeases();
}

void DHCPServer::reclaimExpiredLeases() {
    for (int i = m_leases.size() - 1; i >= 0; --i) {
        if (m_leases[i].leaseExpirationTime <= m_currentTime) {
            qDebug() << "Reclaiming expired IP:" << m_leases[i].ipAddress;
            m_leases.removeAt(i);
        }
    }
}
