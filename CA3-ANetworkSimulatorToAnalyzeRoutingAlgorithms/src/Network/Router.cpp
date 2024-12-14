#include "Router.h"
#include <QDebug>

Router::Router(int id, const QString &ipAddress, int portCount, QObject *parent)
    : Node(id, ipAddress, NodeType::Router, parent),
    m_portCount(portCount),
    m_hasValidIP(false),
    m_assignedIP("")
{
    if (m_portCount <= 0)
    {
        m_portCount = 6;
    }

    initializePorts();

    for (auto &port : m_ports) {
        connect(port.data(), &Port::packetReceived, this, &Router::processPacket);
    }

    qDebug() << "Router initialized: ID =" << m_id << ", IP =" << m_ipAddress << ", Ports =" << m_portCount;
}

Router::~Router()
{
    qDebug() << "Router destroyed: ID =" << m_id;
}

void Router::initializePorts()
{
    for (int i = 0; i < m_portCount; ++i)
    {
        auto port = PortPtr_t::create(nullptr);
        port->setPortNumber(static_cast<uint8_t>(i + 1));
        port->setRouterIP(m_ipAddress);
        m_ports.push_back(port);
    }
}

PortPtr_t Router::getAvailablePort()
{
    for (const auto &port : m_ports)
    {
        if (!port->isConnected())
        {
            return port;
        }
    }
    return nullptr;
}

std::vector<PortPtr_t> Router::getPorts()
{
    return m_ports;
}

void Router::requestIPFromDHCP() {
    if (m_hasValidIP) {
        qDebug() << "Router" << m_id << "already has a valid IP:" << m_assignedIP;
        return;
    }

    auto packet = QSharedPointer<Packet>::create(PacketType::DHCPRequest, QString("DHCP_REQUEST:%1").arg(m_id), 16);
    qDebug() << "Router" << m_id << "created DHCP request with payload:" << packet->getPayload();

    processPacket(packet); // Directly process the packet to start forwarding
}

void Router::processDHCPResponse(const PacketPtr_t &packet)
{
    if (!packet || packet->getPayload().isEmpty()) return;

    if (packet->getPayload().contains("DHCP_OFFER"))
    {
        QStringList parts = packet->getPayload().split(":");
        if (parts.size() == 3)
        {
            QString offeredIP = parts[1];
            int clientId = parts[2].toInt();
            if (clientId == m_id)
            {
                m_assignedIP = offeredIP;
                m_hasValidIP = true;
                qDebug() << "Router" << m_id << "received and assigned IP:" << m_assignedIP;
            }
            else
            {
                qDebug() << "Router" << m_id << "received DHCP offer for client" << clientId << ". Ignoring.";
            }
        }
        else
        {
            qWarning() << "Router" << m_id << "received malformed DHCP offer:" << packet->getPayload();
        }
    }
}

QString Router::getAssignedIP()
{
    return m_assignedIP;
}

void Router::setDHCPServer(QSharedPointer<DHCPServer> dhcpServer)
{
    m_dhcpServer = dhcpServer;
    qDebug() << "Router" << m_id << "configured as DHCP server.";
}

QSharedPointer<DHCPServer> Router::getDHCPServer()
{
    return m_dhcpServer;
}

bool Router::isDHCPServer() const
{
    return !m_dhcpServer.isNull();
}

bool Router::hasSeenPacket(qint64 packetId) const {
    return m_seenPacketIds.contains(packetId);
}

void Router::markPacketAsSeen(qint64 packetId) {
    m_seenPacketIds.insert(packetId);
}

void Router::forwardPacket(const PacketPtr_t &packet) {
    if (!packet) return;

    // Decrement TTL
    packet->decrementTTL();

    if (packet->getTTL() <= 0) {
        qDebug() << "Router" << m_id << "dropping packet due to TTL expiration.";
        return;
    }

    for (auto &port : m_ports) {
        if (port->isConnected()) {
            port->sendPacket(packet);
            qDebug() << "Router" << m_id << "forwarded packet via Port" << port->getPortNumber()
                     << "with TTL:" << packet->getTTL();
        }
    }
}

void Router::logPortStatuses() const
{
    for (const auto &port : m_ports)
    {
        qDebug() << "Port" << port->getPortNumber() << (port->isConnected() ? "Connected" : "Available");
    }
}

void Router::processPacket(const PacketPtr_t &packet) {
    if (!packet) return;
    QString payload = packet->getPayload();
    qDebug() << "Router" << m_id << "processing packet with payload:" << payload;

    if (packet->getTTL() <= 0) {
        qDebug() << "Router" << m_id << "dropping packet due to TTL = 0.";
        return;
    }

    // Track seen packets by unique ID to prevent loops
    if (hasSeenPacket(packet->getId())) {
        qDebug() << "Router" << m_id << "already seen packet ID" << packet->getId() << ", dropping to prevent loops.";
        return;
    }

    markPacketAsSeen(packet->getId());

    if (payload.contains("DHCP_REQUEST")) {
        // DHCP Request Format: "DHCP_REQUEST:<clientId>"
        if (isDHCPServer()) {
            qDebug() << "Router" << m_id << "is a DHCP server. Handling DHCP request.";
            if (m_dhcpServer) {
                m_dhcpServer->receivePacket(packet);
            }
        } else {
            // Not a server, forward the DHCP request
            forwardPacket(packet);
        }
    }
    else if (payload.contains("DHCP_OFFER")) {
        // DHCP Offer Format: "DHCP_OFFER:<ipAddress>:<clientId>"
        QStringList parts = payload.split(":");
        if (parts.size() == 3) {
            QString offeredIP = parts[1];
            int clientId = parts[2].toInt();

            if (clientId == m_id) {
                // This offer is for this router
                qDebug() << "Router" << m_id << "received DHCP offer:" << offeredIP << "for itself. Assigning IP.";
                m_assignedIP = offeredIP;
                m_hasValidIP = true;
                qDebug() << "Router" << m_id << "received and assigned IP:" << m_assignedIP;
                // Do not forward this packet further
            }
            else {
                // Offer not for this router, forward if not seen
                forwardPacket(packet);
            }
        }
        else {
            qWarning() << "Router" << m_id << "received malformed DHCP offer:" << payload;
        }
    }
    else {
        // Unknown packet type
        qDebug() << "Router" << m_id << "received unknown or unsupported packet type. Dropping it.";
    }
}
