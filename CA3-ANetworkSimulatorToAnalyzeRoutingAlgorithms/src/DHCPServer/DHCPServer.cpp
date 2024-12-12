#include "DHCPServer.h"
#include <QDebug>

DHCPServer::DHCPServer(int asId, Port *port, QObject *parent)
    : QObject(parent),
    m_asId(asId),
    m_port(port),
    m_nextAvailableId(1),
    m_currentTime(0)
{
    if (m_asId == 1) {
        m_ipPrefix = "192.168.100.";
    } else if (m_asId == 2) {
        m_ipPrefix = "192.168.200.";
    } else {
        qWarning() << "Unsupported AS ID:" << m_asId;
    }

    connect(m_port, &Port::packetReceived, this, &DHCPServer::receivePacket);
}

DHCPServer::~DHCPServer() {}

void DHCPServer::receivePacket(const PacketPtr_t &packet) {
    qDebug() << "DHCP Server received packet with payload:" << packet->getPayload();
    if (packet->getType() == PacketType::Control && packet->getPayload().contains("DHCP_REQUEST")) {
        assignIP(packet);
    }
}

void DHCPServer::assignIP(const PacketPtr_t &packet) {
    int clientId = packet->getPayload().split(":")[1].toInt();

    for (const auto &lease : m_leases) {
        if (lease.clientId == clientId) {
            qDebug() << "Client already has an IP:" << lease.ipAddress;
            return;
        }
    }

    QString ipAddress = m_ipPrefix + QString::number(m_nextAvailableId);
    DHCPLease lease = {ipAddress, clientId, m_currentTime + LEASE_DURATION};
    m_leases.append(lease);
    m_nextAvailableId++;

    sendOffer(lease);
}

void DHCPServer::sendOffer(const DHCPLease &lease) {
    QString payload = QString("DHCP_OFFER:%1:%2").arg(lease.ipAddress).arg(lease.clientId);
    auto offerPacket = QSharedPointer<Packet>::create(PacketType::Control, payload);
    emit broadcastPacket(offerPacket);
    qDebug() << "Sent DHCP offer:" << payload;
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
