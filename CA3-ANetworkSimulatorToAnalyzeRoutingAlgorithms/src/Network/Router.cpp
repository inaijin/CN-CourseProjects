#include "Router.h"
#include "EventsCoordinator/EventsCoordinator.h"
#include "../Topology/TopologyBuilder.h"
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

void Router::setTopologyBuilder(TopologyBuilder *builder) {
    s_topologyBuilder = builder;
}

TopologyBuilder* Router::s_topologyBuilder = nullptr;

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

    if (packet->getPayload().contains("DHCP_OFFER")) {
        QStringList parts = packet->getPayload().split(":");
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

                addDirectRoute(m_assignedIP, "255.255.255.255");
                qDebug() << "Router" << m_id << "added direct route for its own IP.";
            } else {
                if (!hasSeenPacket(packet)) {
                    markPacketAsSeen(packet);
                    forwardPacket(packet);
                } else {
                    qDebug() << "Router" << m_id << "already seen this DHCP offer, dropping to prevent loops.";
                }
            }
        } else {
            qWarning() << "Malformed DHCP_OFFER packet on Router" << m_id << "payload:" << packet->getPayload();
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

                addDirectRoute(m_assignedIP, "255.255.255.255");
                qDebug() << "Router" << m_id << "added direct route for its own IP.";
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

void Router::addRoute(const QString &destination, const QString &mask, const QString &nextHop, int metric, RoutingProtocol protocol, PortPtr_t learnedFromPort) {
    qDebug() << "Router" << m_id << "addRoute called with:" << destination << mask << nextHop << metric;

    for (auto &entry : m_routingTable) {
        if (entry.isDirect && entry.destination == destination && entry.mask == mask) {
            qDebug() << "Router" << m_id << ": Ignoring learned route to" << destination << "due to direct route.";
            return;
        }
    }

    int newInvalidTimer = RIP_INVALID_TIMER;
    bool found = false;

    for (auto &entry : m_routingTable) {
        if (entry.destination == destination && entry.mask == mask && !entry.isDirect) {

            if (entry.holdDownTimer > 0 && metric >= entry.metric) {
                qDebug() << "Router" << m_id << ": hold-down active for" << destination << ", ignoring equal or worse route.";
                return;
            }

            if (metric < entry.metric) {
                qDebug() << "Router" << m_id << "updated route to" << destination << "with better metric" << metric;
                entry.nextHop = nextHop;
                entry.metric = metric;
                entry.protocol = protocol;
                entry.lastUpdateTime = m_currentTime;
                entry.learnedFromPort = learnedFromPort;
                entry.invalidTimer = newInvalidTimer;
                entry.holdDownTimer = 0;
                entry.flushTimer = 0;
            } else {
                qDebug() << "Router" << m_id << ": got equal or worse metric (" << metric << ") for" << destination << ", ignoring update.";
            }
            return;
        }
    }

    if (!found) {
        RouteEntry newEntry(destination, mask, nextHop, metric, protocol, m_currentTime, learnedFromPort, false);
        newEntry.invalidTimer = newInvalidTimer;
        qDebug() << "Router" << m_id << "added new learned route to" << destination << "metric" << metric;
        m_routingTable.append(newEntry);
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
    // qDebug() << "Router" << m_id << "tick:" << m_currentTime;

    if (m_currentTime - m_lastRIPUpdateTime >= RIP_UPDATE_INTERVAL) {
        qDebug() << "Router" << m_id << "sending RIP update at time:" << m_currentTime;
        sendRIPUpdate();
        m_lastRIPUpdateTime = m_currentTime;
    }

    handleRouteTimeouts();
}

void Router::sendRIPUpdate() {
    for (auto &port : m_ports) {
        if (!port->isConnected()) continue;

        QString payload = "RIP_UPDATE:";
        int routeCount = 0;
        for (const auto &entry : m_routingTable) {
            int advertisedMetric = entry.metric;
            if (entry.learnedFromPort == port && !entry.isDirect) {
                advertisedMetric = RIP_INFINITY;
            }
            payload += entry.destination + "," + entry.mask + "," + QString::number(advertisedMetric) + "#";
            routeCount++;
        }

        payload += m_ipAddress;
        if (routeCount == 0) {
            payload = "RIP_UPDATE:" + m_ipAddress;
        }

        auto updatePacket = QSharedPointer<Packet>::create(PacketType::Control, payload);
        updatePacket->setTTL(10);
        port->sendPacket(updatePacket);
        qDebug() << "Router" << m_id << "sent RIP update via Port" << port->getPortNumber() << "with" << routeCount << "routes";
    }
}

void Router::processRIPUpdate(const PacketPtr_t &packet)
{
    if (!packet) return;

    QString payload = packet->getPayload();
    qDebug() << "Router" << m_id << "processing RIP update packet.";

    auto parts = payload.split(":");
    if (parts.size() < 2) {
        qWarning() << "Router" << m_id << "received malformed RIP update:" << payload;
        return;
    }

    QString data = parts[1];
    auto routesAndSender = data.split("#");
    if (routesAndSender.isEmpty()) return;

    QString senderIP = routesAndSender.last();
    routesAndSender.removeLast();

    // Determine incoming port
    Port *incomingPort = qobject_cast<Port*>(sender());
    PortPtr_t incomingPortPtr;
    if (incomingPort) {
        for (auto &p : m_ports) {
            if (p.data() == incomingPort) {
                incomingPortPtr = p;
                break;
            }
        }
    }

    qDebug() << "Router" << m_id << "RIP update from" << senderIP << "with" << routesAndSender.size() << "routes.";
    for (const auto &routeStr : routesAndSender) {
        if (routeStr.isEmpty()) continue;
        auto fields = routeStr.split(",");
        if (fields.size() < 3) {
            qWarning() << "Router" << m_id << "RIP route entry malformed:" << routeStr;
            continue;
        }

        QString dest = fields[0];
        QString mask = fields[1];
        int recvMetric = fields[2].toInt();
        int newMetric = recvMetric + 1;

        qDebug() << "Router" << m_id << "processing route" << dest << "/" << mask << "from" << senderIP << "metric" << newMetric;

        if (newMetric >= RIP_INFINITY) {
            qDebug() << "Router" << m_id << "received unreachable route for" << dest << "skipping.";
            continue;
        }

        addRoute(dest, mask, senderIP, newMetric, RoutingProtocol::RIP, incomingPortPtr);
    }
}

void Router::handleRouteTimeouts() {
    for (auto &entry : m_routingTable) {
        if (entry.isDirect) continue;

        if (entry.invalidTimer > 0) {
            entry.invalidTimer--;
            if (entry.invalidTimer == 0 && entry.metric < RIP_INFINITY) {
                entry.metric = RIP_INFINITY;
                entry.holdDownTimer = RIP_HOLDOWN_TIMER;
                qDebug() << "Router" << m_id << ": Route to" << entry.destination << "invalidated, starting hold-down.";
            }
        }

        if (entry.holdDownTimer > 0) {
            entry.holdDownTimer--;
            if (entry.holdDownTimer == 0 && entry.metric == RIP_INFINITY) {
                entry.flushTimer = RIP_FLUSH_TIMER;
                qDebug() << "Router" << m_id << ": Hold-down ended for" << entry.destination << ", starting flush timer.";
            }
        }

        if (entry.flushTimer > 0) {
            entry.flushTimer--;
        }
    }

    for (int i = m_routingTable.size() - 1; i >= 0; i--) {
        if (!m_routingTable[i].isDirect && m_routingTable[i].metric == RIP_INFINITY && m_routingTable[i].flushTimer == 0 && m_routingTable[i].holdDownTimer == 0 && m_routingTable[i].invalidTimer == 0) {
            qDebug() << "Router" << m_id << ": Removing fully expired route to" << m_routingTable[i].destination;
            m_routingTable.removeAt(i);
        }
    }
}

void Router::addDirectRoute(const QString &destination, const QString &mask) {
    qDebug() << "Router" << m_id << "adding stable direct route:" << destination << "/" << mask;
    RouteEntry directRoute(destination, mask, destination, 0, RoutingProtocol::RIP, m_currentTime, nullptr, true);
    for (int i = m_routingTable.size() - 1; i >= 0; i--) {
        if (m_routingTable[i].destination == destination && m_routingTable[i].mask == mask && m_routingTable[i].isDirect) {
            m_routingTable.removeAt(i);
        }
    }
    m_routingTable.append(directRoute);
}

void Router::setupDirectNeighborRoutes() {
    auto neighbors = getDirectlyConnectedRouters();
    for (auto &nbr : neighbors) {
        QString nbrIP = nbr->getIPAddress();
        qDebug() << "neighbor IP " << nbrIP;
        if (nbrIP.isEmpty()) {
            qWarning() << "Router" << m_id << ": Neighbor" << nbr->getId() << "has no IP yet.";
            continue;
        }

        qDebug() << "Router" << m_id << "adding direct neighbor route to" << nbrIP;
        RouteEntry directNeighborRoute(nbrIP, "255.255.255.255", nbrIP, 1,
                                       RoutingProtocol::RIP, m_currentTime, nullptr, true);

        for (int i = m_routingTable.size() - 1; i >= 0; i--) {
            if (m_routingTable[i].destination == nbrIP && !m_routingTable[i].isDirect) {
                m_routingTable.removeAt(i);
            }
        }

        bool alreadyExists = false;
        for (auto &entry : m_routingTable) {
            if (entry.isDirect && entry.destination == nbrIP) {
                alreadyExists = true;
                break;
            }
        }

        if (!alreadyExists) {
            m_routingTable.append(directNeighborRoute);
        }
    }
}

std::vector<QSharedPointer<Router>> Router::getDirectlyConnectedRouters() {
    std::vector<QSharedPointer<Router>> neighbors;

    for (auto &port : m_ports) {
        if (port->isConnected()) {
            int remoteId = port->getConnectedRouterId();
            if (remoteId > 0 && remoteId != m_id && s_topologyBuilder) {
                QSharedPointer<Router> nbr = nullptr;
                if (remoteId < 24) {
                    nbr = s_topologyBuilder->findRouterById(remoteId);
                }
                if (nbr) {
                    neighbors.push_back(nbr);
                }
            }
        }
    }

    return neighbors;
}
