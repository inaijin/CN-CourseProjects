#include "Router.h"
#include <QDebug>

Router::Router(int id, const QString &ipAddress, int portCount, QObject *parent)
    : Node(id, ipAddress, NodeType::Router, parent), m_portCount(portCount), m_hasValidIP(false), m_assignedIP("")
{
    if (m_portCount <= 0)
    {
        m_portCount = 6;
    }

    initializePorts();

    // Connect ports' packetReceived signal to Router's processPacket slot
    for (auto &port : m_ports) {
        connect(port.data(), &Port::packetReceived, this, [this](const PacketPtr_t &packet) {
            processPacket(packet);
        });
    }

    qDebug() << "Router initialized: ID =" << m_id << ", IP =" << m_ipAddress << ", Ports =" << m_portCount;
}

Router::~Router()
{
    if (isRunning())
    {
        quit();
        wait();
    }

    qDebug() << "Router destroyed: ID =" << m_id;
}

void Router::initializePorts()
{
    for (int i = 0; i < m_portCount; ++i)
    {
        auto port = PortPtr_t::create(this);
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

void Router::run()
{
    qDebug() << "Router" << m_id << "is in idle mode.";
    exec();
}

void Router::startRouter()
{
    qDebug() << "Starting Router" << m_id;
    if (!isRunning())
    {
        start();
    }
}

void Router::forwardPacket(const PacketPtr_t &packet) {
    qDebug() << "Router" << m_id << "is forwarding packet with payload:" << packet->getPayload();

    for (const auto &port : m_ports) {
        if (port->isConnected()) {
            port->sendPacket(packet);
            qDebug() << "Router" << m_id << "forwarded packet via Port" << port->getPortNumber();
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

void Router::requestIPFromDHCP() {
    if (m_hasValidIP) {
        qDebug() << "Router" << m_id << "already has a valid IP:" << m_assignedIP;
        return;
    }

    auto packet = QSharedPointer<Packet>::create(PacketType::Control, QString("DHCP_REQUEST:%1").arg(m_id));
    qDebug() << "Router" << m_id << "created DHCP request with payload:" << packet->getPayload();

    processPacket(packet); // Process packet internally
}

void Router::processDHCPResponse(const PacketPtr_t &packet)
{
    if (!packet || packet->getPayload().isEmpty()) return;

    if (packet->getPayload().contains("DHCP_OFFER"))
    {
        QStringList parts = packet->getPayload().split(":");
        if (parts.size() >= 2)
        {
            m_assignedIP = parts[1];
            m_hasValidIP = true;
            qDebug() << "Router" << m_id << "received and assigned IP:" << m_assignedIP;
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

bool Router::isDHCPServer() const
{
    return !m_dhcpServer.isNull();
}

QSharedPointer<DHCPServer> Router::getDHCPServer()
{
    return m_dhcpServer;
}

void Router::processPacket(const PacketPtr_t &packet) {
    if (!packet) return;
    qDebug() << "Router" << m_id << "processing packet with payload:" << packet->getPayload();

    // Check TTL
    if (packet->getTTL() <= 0) {
        qDebug() << "Router" << m_id << "dropping packet due to TTL expiration.";
        return;
    }

    QString payload = packet->getPayload();

    if (payload.contains("DHCP_REQUEST")) {
        // DHCP Request
        if (isDHCPServer()) {
            qDebug() << "Router" << m_id << "is a DHCP server. Handling DHCP request.";
            if (m_dhcpServer) {
                m_dhcpServer->receivePacket(packet);
            }
            // After serving the request, do not forward
        } else {
            if (!hasSeenPacket(packet)) {
                markPacketAsSeen(packet);
                packet->decrementTTL();
                forwardPacket(packet);
            } else {
                qDebug() << "Router" << m_id << "already seen this DHCP request, dropping to prevent loops.";
            }
        }
    } else if (payload.contains("DHCP_OFFER")) {
        // DHCP Offer
        QStringList parts = payload.split(":");
        if (parts.size() >= 3) {
            int clientId = parts.last().toInt();
            if (clientId == m_id) {
                // Offer is for this router
                qDebug() << "Router" << m_id << "received DHCP offer:" << payload;
                processDHCPResponse(packet);
            } else {
                // Offer not for this router
                if (!hasSeenPacket(packet)) {
                    markPacketAsSeen(packet);
                    packet->decrementTTL();
                    forwardPacket(packet);
                } else {
                    qDebug() << "Router" << m_id << "already seen this DHCP offer, dropping to prevent loops.";
                }
            }
        } else {
            qWarning() << "Router" << m_id << "received malformed DHCP offer:" << payload;
        }
    } else {
        // Other types of packets: to prevent infinite loops, drop if not recognized
        qDebug() << "Router" << m_id << "received unknown or unsupported packet type, dropping it.";
    }
}
