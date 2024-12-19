#include "Router.h"
#include "EventsCoordinator/EventsCoordinator.h"
#include <QDebug>
#include <QThread>

Router::Router(int id, const QString &ipAddress, int portCount, QObject *parent)
    : Node(id, ipAddress, NodeType::Router, parent),
    m_portCount(portCount),
    m_hasValidIP(false),
    m_lastRIPUpdateTime(0),
    m_currentTime(0)
{
    if (m_portCount <= 0)
    {
        m_portCount = 6;
    }

    initializePorts();

    for (auto &port : m_ports) {
        connect(port.data(), &Port::packetReceived, this, [this](const PacketPtr_t &packet) {
            processPacket(packet);
        });
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

void Router::initialize()
{
    qDebug() << "Router initialized: ID =" << m_id << ", IP =" << m_ipAddress
             << ", running in thread" << (quintptr)QThread::currentThreadId();
}

void Router::forwardPacket(const PacketPtr_t &packet) {
    if (!packet) return;
    if (packet->getTTL() <= 0) {
        qDebug() << "Router" << m_id << "dropping packet due to TTL expiration.";
        return;
    }

    PacketPtr_t fwdPacket(new Packet(packet->getType(), packet->getPayload(), packet->getTTL()-1));

    for (auto &port : m_ports) {
        if (port->isConnected()) {
            port->sendPacket(fwdPacket);
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

    processPacket(packet);
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

bool Router::hasSeenPacket(const PacketPtr_t &packet) {
    return m_seenPackets.contains(packet->getPayload());
}

void Router::markPacketAsSeen(const PacketPtr_t &packet) {
    m_seenPackets.insert(packet->getPayload());
}

void Router::processPacket(const PacketPtr_t &packet) {
    if (!packet) return;
    QString payload = packet->getPayload();
    qDebug() << "Router" << m_id << "processing packet with payload:" << payload;

    if (packet->getTTL() <= 0) {
        qDebug() << "Router" << m_id << "dropping packet due to TTL = 0.";
        return;
    }

    if (payload.contains("DHCP_REQUEST")) {
        if (isDHCPServer()) {
            if (m_dhcpServer) {
                m_dhcpServer->receivePacket(packet);
            }
        } else {
            if (!hasSeenPacket(packet)) {
                markPacketAsSeen(packet);
                forwardPacket(packet);
            } else {
                qDebug() << "Router" << m_id << "already seen this DHCP request, dropping to prevent loops.";
            }
        }
    } else if (payload.contains("DHCP_OFFER")) {
        QStringList parts = payload.split(":");
        if (parts.size() == 3) {
            QString offeredIP = parts[1];
            int clientId = parts[2].toInt();

            if (clientId == m_id) {
                if (m_hasValidIP) {
                    qDebug() << "Router" << m_id << "already has a valid IP:" << m_assignedIP;
                    return;
                }
                qDebug() << "Router" << m_id << "received DHCP offer:" << offeredIP << "for itself. Assigning IP.";
                m_assignedIP = offeredIP;
                m_ipAddress = m_assignedIP;
                m_hasValidIP = true;
                qDebug() << "Router" << m_id << "received and assigned IP:" << m_assignedIP;
            } else {
                if (!hasSeenPacket(packet)) {
                    markPacketAsSeen(packet);
                    forwardPacket(packet);
                } else {
                    qDebug() << "Router" << m_id << "already seen this DHCP offer, dropping to prevent loops.";
                }
            }
        } else {
            qWarning() << "Malformed DHCP_OFFER packet on Router" << m_id << "payload:" << payload;
        }
    }
    else if (payload.startsWith("RIP_UPDATE")) {
        processRIPUpdate(packet);
    } else {
        qDebug() << "Router" << m_id << "received unknown/unsupported packet:" << payload << "Dropping it.";
    }
}

void Router::addRoute(const QString &destination, const QString &mask, const QString &nextHop, int metric, RoutingProtocol protocol)
{
    bool updated = false;
    qint64 now = m_currentTime;

    for (auto &entry : m_routingTable) {
        if (entry.destination == destination && entry.mask == mask) {
            if (entry.metric <= metric) {
                return;
            } else {
                entry.nextHop = nextHop;
                entry.metric = metric;
                entry.protocol = protocol;
                entry.lastUpdateTime = now;
                qDebug() << "Router" << m_id << ": Updated route to" << destination << "metric" << metric;
                updated = true;
                break;
            }
        }
    }

    if (!updated) {
        RouteEntry newEntry(destination, mask, nextHop, metric, protocol, now);
        m_routingTable.append(newEntry);
        qDebug() << "Router" << m_id << ": Added route to" << destination << "metric" << metric;
    }
}

QString Router::findBestRoute(const QString &destinationIP) const
{
    int bestMetric = INT_MAX;
    QString bestNextHop = "";

    for (const auto &route : m_routingTable) {
        if (route.destination == destinationIP && route.metric < bestMetric) {
            bestMetric = route.metric;
            bestNextHop = route.nextHop;
        }
    }

    return bestNextHop;
}

void Router::printRoutingTable() const
{
    qDebug() << "Routing Table for Router" << m_id << ":";
    for (const auto &entry : m_routingTable) {
        QString protoStr = (entry.protocol == RoutingProtocol::RIP) ? "RIP" : "OSPF";
        qDebug() << "Dest:" << entry.destination
                 << "Mask:" << entry.mask
                 << "NextHop:" << entry.nextHop
                 << "Metric:" << entry.metric
                 << "Protocol:" << protoStr
                 << "LastUpdated:" << entry.lastUpdateTime;
    }
}

void Router::enableRIP()
{
    connect(EventsCoordinator::instance(), &EventsCoordinator::tick, this, &Router::onTick);
    qDebug() << "RIP enabled on Router" << m_id;
}

void Router::onTick()
{
    m_currentTime++;

    if (m_currentTime - m_lastRIPUpdateTime >= RIP_UPDATE_INTERVAL && m_hasValidIP) {
        sendRIPUpdate();
        m_lastRIPUpdateTime = m_currentTime;
    }

    handleRouteTimeouts(m_currentTime);
}

void Router::sendRIPUpdate()
{
    QString payload = "RIP_UPDATE:";
    int routeCount = 0;
    for (auto &entry : m_routingTable) {
        if (entry.metric < RIP_INFINITY) {
            payload += entry.destination + "," + entry.mask + "," + QString::number(entry.metric) + "#";
            routeCount++;
        }
    }

    payload += m_ipAddress;

    if (routeCount == 0) {
        payload = "RIP_UPDATE:" + m_ipAddress;
    }

    auto updatePacket = QSharedPointer<Packet>::create(PacketType::Control, payload);
    updatePacket->setTTL(10);

    for (auto &port : m_ports) {
        if (port->isConnected()) {
            port->sendPacket(updatePacket);
        }
    }

    qDebug() << "Router" << m_id << "sent RIP update with" << routeCount << "routes.";
}

void Router::processRIPUpdate(const QSharedPointer<Packet> &packet)
{
    QString payload = packet->getPayload();
    auto parts = payload.split(":");
    if (parts.size() < 2) return;

    QString data = parts[1];
    auto routesAndSender = data.split("#");
    if (routesAndSender.isEmpty()) return;

    QString senderIP = routesAndSender.last();
    routesAndSender.removeLast();

    for (const auto &routeStr : routesAndSender) {
        if (routeStr.isEmpty()) continue;
        auto fields = routeStr.split(",");
        if (fields.size() < 3) continue;

        QString dest = fields[0];
        QString mask = fields[1];
        int recvMetric = fields[2].toInt();

        int newMetric = recvMetric + 1;
        if (newMetric >= RIP_INFINITY) {
            continue;
        }

        addRoute(dest, mask, senderIP, newMetric, RoutingProtocol::RIP);
    }
}

void Router::handleRouteTimeouts(qint64 currentTime)
{
    for (auto &entry : m_routingTable) {
        if (entry.protocol == RoutingProtocol::RIP) {
            if ((currentTime - entry.lastUpdateTime) > RIP_ROUTE_TIMEOUT && entry.metric < RIP_INFINITY) {
                entry.metric = RIP_INFINITY;
                qDebug() << "Router" << m_id << ": Route to" << entry.destination << "timed out, marking unreachable.";
            }
        }
    }
}
