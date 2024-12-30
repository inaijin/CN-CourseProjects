#include "DHCPServer.h"
#include "../IP/IP.h"
#include "../IP/IPHeader.h"
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
    QStringList parts = packet->getPayload().split(":");
    if (parts.size() < 2) {
        qWarning() << "Malformed DHCP_REQUEST packet.";
        return;
    }

    int clientId = parts[1].toInt();

    if (m_asId == 1) {
        if (!((clientId >= 1 && clientId <= 16) || (clientId >= 24 && clientId <= 31))) {
            qDebug() << "Client" << clientId << "Not In Our AS(1)";
            return;
        }
    } else if (m_asId == 2) {
        if (!((clientId >= 17 && clientId <= 23) || (clientId >= 32 && clientId <= 38))) {
            qDebug() << "Client" << clientId << "Not In Our AS(2)";
            return;
        }
    } else {
        qWarning() << "Unsupported AS ID:" << m_asId;
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

    QString ipAddress = QString("%1%2").arg(m_ipPrefix).arg(clientId);

    if (packet->isIPv6()) {
        QString assignedIpAddress = ipAddress;
        qDebug() << "Packet indicates IPv6. Converting assigned IPv4 address to IPv6.";

        QSharedPointer<IPv4Header> ipv4Header = QSharedPointer<IPv4Header>::create();
        ipv4Header->setSourceAddress(ipAddress);
        ipv4Header->setDestinationAddress("::");

        IP ip{ QSharedPointer<AbstractIPHeader>(ipv4Header), nullptr };

        if (!ip.convertToIPv6()) {
            qWarning() << "Failed to convert IPv4 address to IPv6 for client" << clientId;
            return;
        }

        assignedIpAddress = ip.getIp();

        qDebug() << "Converted IPv6 Address:" << assignedIpAddress;

        for (const auto &lease : m_leases) {
            if (lease.ipAddress == assignedIpAddress) {
                qWarning() << "IPv6 address" << assignedIpAddress << "is already assigned. Cannot assign to client" << clientId;
                return;
            }
        }

        DHCPLease newLease;
        newLease.ipAddress = assignedIpAddress;
        newLease.clientId = clientId;

        m_leases.append(newLease);

        qDebug() << "New IP assigned:" << newLease.ipAddress << "for client" << clientId;

        sendOffer(newLease);
        return;
    }

    for (const auto &lease : m_leases) {
        if (lease.ipAddress == ipAddress) {
            qWarning() << "IP address" << ipAddress << "is already assigned. Cannot assign to client" << clientId;
            return;
        }
    }

    DHCPLease lease = {ipAddress, clientId, m_currentTime + LEASE_DURATION};
    m_leases.append(lease);

    qDebug() << "New IP assigned:" << ipAddress << "for client" << clientId;
    sendOffer(lease);
}

void DHCPServer::sendOffer(const DHCPLease &lease) {
    QString payload = QString("DHCP_OFFER:%1:%2").arg(lease.ipAddress).arg(lease.clientId);
    auto offerPacket = QSharedPointer<Packet>::create(PacketType::Control, payload);

    offerPacket->setTTL(10); // Default TTL For Offer DHCP Server

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
