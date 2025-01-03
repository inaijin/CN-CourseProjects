#include "Router.h"
#include "EventsCoordinator/EventsCoordinator.h"
#include "../Topology/TopologyBuilder.h"
#include "../MetricsCollector/MetricsCollector.h"
#include "../Globals/RouterRegistry.h"
#include "../Network/PC.h"
#include "../MACAddress/MACADdressGenerator.h"
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

Router::Router(int id, const QString &ipAddress, int portCount, QObject *parent, bool isBroken)
    : Node(id, ipAddress, NodeType::Router, parent),
    m_portCount(portCount),
    m_hasValidIP(false),
    m_lsdb(),
    m_lsaSequenceNumber(0),
    m_bufferSize(10),
    m_bufferRetentionTime(1000),
    m_lastRIPUpdateTime(0),
    m_currentTime(0),
    m_isBroken(isBroken)
{
    if (m_portCount <= 0)
    {
        m_portCount = 6;
    }

    m_ASnum = -1;

    initializePorts();

    m_helloTimer = new QTimer(this);
    connect(m_helloTimer, &QTimer::timeout, this, &Router::sendOSPFHello);

    m_lsaTimer = new QTimer(this);
    connect(m_lsaTimer, &QTimer::timeout, this, &Router::sendLSA);

    for (auto &port : m_ports) {
        connect(port.data(), &Port::packetReceived, this, [this, port](const PacketPtr_t &packet) {
            processPacket(packet, port);
        });
    }

    m_bufferTimer = new QTimer(this);
    connect(m_bufferTimer, &QTimer::timeout, this, &Router::processBuffer);
    m_bufferTimer->start(1000);

    QSharedPointer<MACAddressGenerator> generator = QSharedPointer<MACAddressGenerator>::create();
    m_macAddress = generator->generate();

    qDebug() << "Router initialized: ID =" << m_id << ", IP =" << m_ipAddress->getIp() << ", Ports =" << m_portCount;
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
        port->setRouterIP(m_ipAddress->getIp());
        m_ports.push_back(port);

        QSharedPointer<PC> connectedPC = port->getConnectedPC();
        if (connectedPC)
        {
            addConnectedPC(connectedPC, port);
            port->setConnectedRouterIP(connectedPC->getIpAddress());
        }
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

void Router::setMetricsCollector(QSharedPointer<MetricsCollector> collector) {
    m_metricsCollector = collector;
}

void Router::initialize()
{
    // qDebug() << "Router initialized: ID =" << m_id << ", IP =" << m_ipAddress->getIp()
    //          << ", running in thread" << (quintptr)QThread::currentThreadId();

    startTimers();
}

int Router::IBGPCounter = 0;

void Router::sendHelloPackets()
{
    // qDebug() << "Router" << m_id << "sending OSPF Hello packets.";
    // Implement the logic if needed
}

void Router::onFinished()
{
    emit finished();
}

void Router::startTimers()
{
    // QTimer *timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, this, &Router::sendHelloPackets);
    // timer->start(HELLO_INTERVAL);
}

bool Router::enqueuePacketToBuffer(const PacketPtr_t &packet) {
    QMutexLocker locker(&m_bufferMutex);
    if (m_buffer.size() >= m_bufferSize) {
        qWarning() << "Router" << m_id << ": Buffer full. Dropping packet with payload:" << packet->getPayload();
        if (m_metricsCollector) {
            m_metricsCollector->recordPacketDropped();
        }
        return false;
    }
    BufferedPacket bp;
    bp.packet = packet;
    bp.enqueueTime = QDateTime::currentMSecsSinceEpoch();
    m_buffer.enqueue(bp);
    // qDebug() << "Router" << m_id << ": Packet enqueued. Current buffer size:" << m_buffer.size();
    return true;
}

PacketPtr_t Router::dequeuePacketFromBuffer() {
    QMutexLocker locker(&m_bufferMutex);
    if (m_buffer.isEmpty()) {
        return nullptr;
    }
    BufferedPacket bp = m_buffer.dequeue();
    // qDebug() << "Router" << m_id << ": Packet dequeued. Current buffer size:" << m_buffer.size();
    return bp.packet;
}

void Router::processBuffer() {
    QMutexLocker locker(&m_bufferMutex);
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    while (!m_buffer.isEmpty()) {
        BufferedPacket bp = m_buffer.head();
        if ((currentTime - bp.enqueueTime) > m_bufferRetentionTime) {
            m_buffer.dequeue();
            // qWarning() << "Router" << m_id << ": Packet expired and removed from buffer with payload:" << bp.packet->getPayload();
            if (m_metricsCollector) {
                m_metricsCollector->recordPacketDropped();
            }
        } else {
            break;
        }
    }
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

void Router::addConnectedPC(QSharedPointer<PC> pc, PortPtr_t port)
{
    m_connectedPCs.push_back(pc);
    QString pcIP = pc->getIpAddress();
    qDebug() << "Router" << m_id << "adding route for connected PC:" << pcIP;

    addRoute(pcIP, "255.255.255.255", m_ipAddress->getIp(), 1, RoutingProtocol::RIP, port);
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

    processPacket(packet, nullptr);
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
                m_ipAddress->setIp(m_assignedIP);
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

void Router::processPacket(const PacketPtr_t &packet, const PortPtr_t &incomingPort) {
    if (m_isBroken) {
        m_metricsCollector->recordPacketDropped();
        return;
    }

    if (!packet) return;
    packet->increamentTotalCycle();

    bool enqueued = enqueuePacketToBuffer(packet);
    packet->increamentWaitCycle();

    QString payload = packet->getPayload();
    qDebug() << "Router" << m_id << "processing packet with payload:" << payload;

    // Check and handle TTL
    if (packet->getTTL() <= 0) {
        qDebug() << "Router" << m_id << "dropping packet due to TTL = 0.";
        if (m_metricsCollector &&
           !payload.contains("DHCP_REQUEST") &&
           !payload.contains("DHCP_OFFER") &&
           !payload.startsWith("RIP_UPDATE") &&
           packet->getType() != PacketType::OSPFHello &&
           packet->getType() != PacketType::OSPFLSA) {
            m_metricsCollector->recordPacketDropped();
        }
        dequeuePacketFromBuffer();
        if (m_metricsCollector)
            m_metricsCollector->recordWaitCycle(packet->getWaitingCycle());
        return;
    }

    // Handle DHCP Requests
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
    }
    // Handle DHCP Offers
    else if (payload.contains("DHCP_OFFER")) {
        QStringList parts = payload.split(":");
        if (parts.size() == 3) {
            QString offeredIP = parts[1];
            int clientId = parts[2].toInt();

            if (clientId == m_id) {
                if (m_hasValidIP) {
                    qDebug() << "Router" << m_id << "already has a valid IP:" << m_assignedIP;
                    dequeuePacketFromBuffer();
                    if (m_metricsCollector)
                        m_metricsCollector->recordWaitCycle(packet->getWaitingCycle());
                    return;
                }
                qDebug() << "Router" << m_id << "received DHCP offer:" << offeredIP << "for itself. Assigning IP.";
                m_assignedIP = offeredIP;
                m_ipAddress->setIp(m_assignedIP);
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
    // Handle RIP Updates
    else if (payload.startsWith("RIP_UPDATE")) {
        processRIPUpdate(packet);
    }
    // Handle EBGP Updates
    else if (payload.startsWith("EBGP_UPDATE")) {
        processEBGPUpdate(packet);
    }
    // Handle IBGP Updates
    else if (payload.startsWith("IBGP_UPDATE")) {
        processIBGPUpdate(packet);
    }
    // Handle OSPF Updates
    else if (packet->getType() == PacketType::OSPFHello) {
         processOSPFHello(packet);
    }
    // Handle OSPF LSA Packets
    else if (packet->getType() == PacketType::OSPFLSA) {
        processLSA(packet, incomingPort);
    }
    // Handle Data Packets
    else if (packet->getType() == PacketType::Data) {
        QStringList parts = payload.split(":");
        if (parts.size() >= 3 && parts.at(0) == "Data") {
            QString destinationIP = parts.at(1);
            QString actualPayload = parts.at(2);

            if (destinationIP == m_ipAddress->getIp()) {
                qDebug() << "Router" << m_id << "received packet intended for itself.";

                if (m_metricsCollector) {
                    m_metricsCollector->recordPacketReceived(packet->getPath());
                }

                qDebug() << "Router" << m_id << "processing payload:" << actualPayload;
            }
            else {
                RouteEntry bestRoute = findBestRoutePath(destinationIP);
                if (bestRoute.destination.isEmpty()) {
                    qDebug() << "Router" << m_id << "has no route to destination IP:" << destinationIP << ". Dropping packet.";
                    if (m_metricsCollector) {
                        m_metricsCollector->recordPacketDropped();
                    }
                    dequeuePacketFromBuffer();
                    if (m_metricsCollector)
                        m_metricsCollector->recordWaitCycle(packet->getWaitingCycle());
                    return;
                }

                if (bestRoute.destination == bestRoute.nextHop) {
                    qDebug() << "Router" << m_id << "received packet intended for its PC.";

                    if (m_metricsCollector) {
                        m_metricsCollector->increamentHops();
                        m_metricsCollector->recordPacketReceived(packet->getPath());
                        m_metricsCollector->increamentHops();
                    }

                    packet->addToPathTaken(bestRoute.destination);
                    qDebug() << "PC" << destinationIP << "processing payload:" << actualPayload;
                    qDebug() << "Packet with source" << packet->getPath()[0] << "with destination" << packet->getPath()[1]
                             << "with total wait cycle" << packet->getWaitingCycle() << "and it's total cycle is"
                             << packet->getTotalCycle() << "and it's path taken is" << packet->getPathTaken();
                    dequeuePacketFromBuffer();
                    if (m_metricsCollector)
                        m_metricsCollector->recordWaitCycle(packet->getWaitingCycle());
                    return;
                }

                packet->decrementTTL();
                if (packet->getTTL() <= 0) {
                    qDebug() << "Router" << m_id << "dropping packet due to TTL = 0 after decrement.";
                    if (m_metricsCollector) {
                        m_metricsCollector->recordPacketDropped();
                    }
                    dequeuePacketFromBuffer();
                    if (m_metricsCollector)
                        m_metricsCollector->recordWaitCycle(packet->getWaitingCycle());
                    return;
                }

                packet->addToPath(m_ipAddress->getIp());

                if (m_metricsCollector) {
                    m_metricsCollector->recordRouterUsage(m_ipAddress->getIp());
                }

                PortPtr_t outPort = bestRoute.learnedFromPort;
                if (outPort && outPort->isConnected()) {
                    if (m_metricsCollector) {
                        m_metricsCollector->increamentHops();
                    }
                    packet->addToPathTaken(bestRoute.nextHop);
                    outPort->sendPacket(packet);
                    qDebug() << "Router" << m_id << "forwarded packet to next hop via Port" << outPort->getPortNumber();
                }
                else {
                    qDebug() << "Router" << m_id << "has no valid outgoing port to forward the packet. Dropping packet.";
                    if (m_metricsCollector) {
                        m_metricsCollector->recordPacketDropped();
                    }
                }
            }
        }
        else {
            qWarning() << "Malformed Data packet on Router" << m_id << "payload:" << payload;
            if (m_metricsCollector) {
                m_metricsCollector->recordPacketDropped();
            }
        }
    }
    else {
        qDebug() << "Router" << m_id << "received unknown/unsupported packet:" << payload << "Dropping it.";
        if (m_metricsCollector) {
            m_metricsCollector->recordPacketDropped();
        }
    }

    if (enqueued)
        PacketPtr_t nextPacket = dequeuePacketFromBuffer();
    if (m_metricsCollector)
        m_metricsCollector->recordWaitCycle(packet->getWaitingCycle());
}

void Router::addRoute(const QString &destination, const QString &mask, const QString &nextHop, int metric, RoutingProtocol protocol, PortPtr_t learnedFromPort, bool vip) {
    // qDebug() << "Router" << m_id << "addRoute called with:" << destination << mask << nextHop << metric;

    for (auto &entry : m_routingTable) {
        if (!vip) {
            if (entry.isDirect && entry.destination == destination && entry.mask == mask) {
                qDebug() << "Router" << m_id << ": Ignoring learned route to" << destination << "due to direct route.";
                IBGPCounter += 1;
                return;
            }
        }
    }

    int newInvalidTimer = RIP_INVALID_TIMER;
    bool found = false;

    for (auto &entry : m_routingTable) {
        if (entry.destination == destination && entry.mask == mask && !entry.isDirect) {

            if (entry.holdDownTimer > 0 && metric >= entry.metric) {
                // qDebug() << "Router" << m_id << ": hold-down active for" << destination << ", ignoring equal or worse route.";
                IBGPCounter += 1;
                return;
            }

            if (metric < entry.metric) {
                // qDebug() << "Router" << m_id << "updated route to" << destination << "with better metric" << metric;
                entry.nextHop = nextHop;
                entry.metric = metric;
                entry.protocol = protocol;
                entry.lastUpdateTime = m_currentTime;
                entry.learnedFromPort = learnedFromPort;
                entry.invalidTimer = newInvalidTimer;
                entry.holdDownTimer = 0;
                entry.flushTimer = 0;
            } else {
                // qDebug() << "Router" << m_id << ": got equal or worse metric (" << metric << ") for" << destination << ", ignoring update.";
            }
            IBGPCounter += 1;
            return;
        }
    }

    if (!found) {
        RouteEntry newEntry(destination, mask, nextHop, metric, protocol, m_currentTime, learnedFromPort, false);
        newEntry.invalidTimer = newInvalidTimer;
        // qDebug() << "Router" << m_id << "added new learned route to" << destination << "metric" << metric;
        m_routingTable.append(newEntry);
        emit routingTableUpdated(m_id);
        IBGPCounter = 0;
    }
}

RouteEntry Router::findBestRoutePath(const QString &destinationIP) const {
    RouteEntry bestRoute;
    int minMetric = RIP_INFINITY;
    QString bgpDest = " III ";

    if(m_ASnum != -1)
        bgpDest = (m_ASnum == 1) ? "192.168.200.xx" : "192.168.100.xx";

    QString bgpPrefix;
    if (bgpDest.endsWith(".xx")) {
        bgpPrefix = bgpDest.left(bgpDest.length() - 3);
    } else {
        bgpPrefix = bgpDest;
    }

    for (const auto &route : m_routingTable) {
        if (destinationIP == route.destination) {
            if (route.metric < minMetric) {
                minMetric = route.metric;
                bestRoute = route;
            }
        } else if (route.destination == bgpDest) {
            if (destinationIP.startsWith(bgpPrefix + ".")) {
                minMetric = route.metric;
                bestRoute = route;
            }
        }
    }

    return bestRoute;
}

void Router::printRoutingTable() const
{
    QMutexLocker locker(&m_logMutex);

    qDebug() << "Routing Table for Router" << m_id << ":";

    QString routerIdStr;

    routerIdStr = QString::number(m_id);

    QString logFilePath = QString("D:/QTProjects/GitHub/CN-CourseProjects/CA3-ANetworkSimulatorToAnalyzeRoutingAlgorithms/logs/routingTableRouter%1.txt")
                            .arg(routerIdStr);

    QFileInfo fileInfo(logFilePath);
    QDir logDir = fileInfo.dir();
    if (!logDir.exists()) {
        if (!logDir.mkpath(".")) {
            qWarning() << "Failed to create log directory:" << logDir.absolutePath();
            return;
        }
    }

    QFile logFile(logFilePath);
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "Cannot open log file:" << logFilePath;
        return;
    }

    QTextStream out(&logFile);

    QString headerTimeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    out << headerTimeStamp << " Routing Table for Router " << m_id << ":\n";

    for (const auto &entry : m_routingTable) {
        QString protoStr;
        switch (entry.protocol) {
            case RoutingProtocol::RIP:
                protoStr = "RIP";
                break;
            case RoutingProtocol::OSPF:
                protoStr = "OSPF";
                break;
            case RoutingProtocol::ITSELF:
                protoStr = "ITSELF";
                break;
            case RoutingProtocol::EBGP:
                protoStr = "EBGP";
                break;
            case RoutingProtocol::IBGP:
                protoStr = "IBGP";
                break;
            default:
                protoStr = "UNKNOWN";
                break;
        }

        QString logEntry;
        QTextStream(&logEntry) << "Dest: " << entry.destination
                               << " Mask: " << entry.mask
                               << " NextHop: " << entry.nextHop
                               << " Metric: " << entry.metric
                               << " Protocol: " << protoStr;

        qDebug() << logEntry;

        QString entryTimeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        out << entryTimeStamp << " " << logEntry << "\n";
    }

    logFile.close();
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
        // qDebug() << "Router" << m_id << "sending RIP update at time:" << m_currentTime;
        sendRIPUpdate();
        m_lastRIPUpdateTime = m_currentTime;
    }

    handleRouteTimeouts();
}

void Router::sendRIPUpdate() {
    for (auto &port : m_ports) {
        if (m_ASnum != -1) {
            Range range = getRange(m_ASnum);
            int connectedRouterId = port->getConnectedRouterId();
            if (connectedRouterId <= range.pcMax && connectedRouterId >= range.pcMin) {

            } else if (connectedRouterId > range.max || connectedRouterId < range.min) {
                continue;
            }
        }

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

        payload += m_ipAddress->getIp();
        if (routeCount == 0) {
            payload = "RIP_UPDATE:" + m_ipAddress->getIp();
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
    // qDebug() << "Router" << m_id << "processing RIP update packet.";

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

    // qDebug() << "Router" << m_id << "RIP update from" << senderIP << "with" << routesAndSender.size() << "routes.";
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

        // qDebug() << "Router" << m_id << "processing route" << dest << "/" << mask << "from" << senderIP << "metric" << newMetric;

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
    RouteEntry directRoute(destination, mask, destination, 0, RoutingProtocol::ITSELF, m_currentTime, nullptr, true);
    for (int i = m_routingTable.size() - 1; i >= 0; i--) {
        if (m_routingTable[i].destination == destination && m_routingTable[i].mask == mask && m_routingTable[i].isDirect) {
            m_routingTable.removeAt(i);
        }
    }
    m_routingTable.append(directRoute);
}

void Router::setupDirectNeighborRoutes(RoutingProtocol protocol, int ASId, bool bgp) {
    auto neighbors = getDirectlyConnectedRouters(ASId, bgp);
    for (auto &nbr : neighbors) {
        QString nbrIP = nbr->getIPAddress();
        qDebug() << "neighbor IP " << nbrIP;
        if (nbrIP.isEmpty()) {
            qWarning() << "Router" << m_id << ": Neighbor" << nbr->getId() << "has no IP yet.";
            continue;
        }

        qDebug() << "Router" << m_id << "adding direct neighbor route to" << nbrIP;
        RouteEntry directNeighborRoute(nbrIP, "255.255.255.255", nbrIP, 1,
                                       protocol, m_currentTime, nullptr, true);

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
    if (protocol == RoutingProtocol::OSPF) {
        std::vector<QSharedPointer<Router>> connectedPCs = getDirectlyConnectedRouters(ASId, bgp);

        for (auto &pc : connectedPCs) {
            if (pc->getId() > 23) {
                QString pcIP = pc->getIPAddress();
                PortPtr_t learnedFromPort = nullptr;
                for (auto &port : m_ports) {
                    if (port->getConnectedPC() != nullptr && port->getConnectedPC()->getIpAddress() == pcIP)
                        learnedFromPort = port;
                }
                addRoute(pcIP, "255.255.255.255", pcIP, 1, RoutingProtocol::OSPF, learnedFromPort, true);
                RouteEntry directRoute(pcIP, "255.255.255.255", pcIP, 1, RoutingProtocol::OSPF, m_currentTime, nullptr, true, true);
                for (int i = m_routingTable.size() - 1; i >= 0; i--) {
                    if (m_routingTable[i].destination == pcIP && m_routingTable[i].mask == "255.255.255.255" && m_routingTable[i].isDirect) {
                        m_routingTable.removeAt(i);
                    }
                }
                m_routingTable.append(directRoute);
            }
        }
    }
}

std::vector<QSharedPointer<Router>> Router::getDirectlyConnectedRouters(int ASId, bool bgp) {
    Range range = {0, 0, 0, 0};
    if (bgp) {
        range = getRange(ASId);
    }

    std::vector<QSharedPointer<Router>> neighbors;
    for (auto &port : m_ports) {
        if (port->isConnected()) {
            int remoteId = port->getConnectedRouterId();
            QSharedPointer<PC> pc = port->getConnectedPC();

            if (bgp) {
                if (remoteId <= range.pcMax && remoteId >= range.pcMin) {
                } else if (remoteId > range.max || remoteId < range.min) {
                    continue;
                }
            }

            if (remoteId > 0 && remoteId != m_id && s_topologyBuilder && remoteId < 24) {
                QSharedPointer<Router> nbr = nullptr;
                if (remoteId < 24) {
                    nbr = s_topologyBuilder->findRouterById(remoteId);
                }
                if (nbr && !nbr->isBroken()) {
                    neighbors.push_back(nbr);
                }
            } else if (!pc.isNull()) {
                port->setConnectedRouterIP(pc->getIpAddress());
                neighbors.push_back(QSharedPointer<Router>::create(pc->getId(), pc->getIpAddress()));
            }
        }
    }

    return neighbors;
}

void Router::enableOSPF()
{
    initializeOSPF();
    qDebug() << "OSPF enabled on Router" << m_id;
}

void Router::initializeOSPF()
{
    sendOSPFHello();

    m_helloTimer->start(OSPF_HELLO_INTERVAL * 1000);
    m_lsaTimer->start(OSPF_LSA_INTERVAL * 1000);

    connect(EventsCoordinator::instance(), &EventsCoordinator::tick, this, &Router::handleLSAExpiration);
}

void Router::sendOSPFHello()
{
    qDebug() << "Router" << m_id << "sending OSPF Hello packets.";

    for (const auto &port : m_ports)
    {
        if (m_ASnum != -1) {
            Range range = getRange(m_ASnum);
            int connectedRouterId = port->getConnectedRouterId();
            if (connectedRouterId <= range.pcMax && connectedRouterId >= range.pcMin) {
            } else if (connectedRouterId > range.max || connectedRouterId < range.min) {
                continue;
            }
        }

        if (!port->isConnected()) continue;

        if (port->getConnectedRouterIP().isEmpty()) continue;

        QString helloPayload = "OSPF_HELLO:" + m_ipAddress->getIp();
        auto helloPacket = QSharedPointer<Packet>::create(PacketType::OSPFHello, helloPayload, 10);

        port->sendPacket(helloPacket);
        qDebug() << "Router" << m_id << "sent OSPF Hello via Port" << port->getPortNumber();
    }
}

void Router::processOSPFHello(const PacketPtr_t &packet)
{
    if (!packet) return;

    QString payload = packet->getPayload();
    QStringList parts = payload.split(":");
    if (parts.size() != 2 || parts[0] != "OSPF_HELLO")
    {
        qWarning() << "Router" << m_id << "received malformed OSPF Hello packet.";
        return;
    }

    QString neighborIP = parts[1];
    qDebug() << "Router" << m_id << "received OSPF Hello from" << neighborIP;

    if (!m_neighbors.contains(neighborIP))
    {
        OSPFNeighbor neighbor;
        neighbor.ipAddress = neighborIP;
        neighbor.cost = 1;
        neighbor.lastHelloReceived = QDateTime::currentSecsSinceEpoch();

        m_neighbors.insert(neighborIP, neighbor);

        qDebug() << "Router" << m_id << "added new OSPF neighbor:" << neighborIP;

        sendLSA();
    }
    else
    {
        m_neighbors[neighborIP].lastHelloReceived = QDateTime::currentSecsSinceEpoch();
        qDebug() << "Router" << m_id << "updated lastHelloReceived for neighbor:" << neighborIP;
    }
}

void Router::sendLSA()
{
    qDebug() << "Router" << m_id << "sending LSA.";

    QString lsaPayload = "LSA:" + m_ipAddress->getIp() + ":";

    for (auto &neighbor : m_neighbors)
    {
        lsaPayload += neighbor.ipAddress + ",";
    }

    if (lsaPayload.endsWith(","))
        lsaPayload.chop(1);

    m_lsaSequenceNumber++;

    auto lsaPacket = QSharedPointer<Packet>::create(PacketType::OSPFLSA, lsaPayload, 10);
    lsaPacket->setSequenceNumber(m_lsaSequenceNumber);

    OSPFLSA lsa;
    lsa.originRouterIP = m_ipAddress->getIp();
    lsa.links = QVector<QString>::fromList(m_neighbors.keys());
    lsa.sequenceNumber = m_lsaSequenceNumber;
    lsa.age = 0;

    m_lsdb.insert(m_ipAddress->getIp(), lsa);

    for (const auto &port : m_ports)
    {
        if (!port->isConnected()) continue;

        if (port->getConnectedRouterIP().isEmpty()) continue;

        port->sendPacket(lsaPacket);
        qDebug() << "Router" << m_id << "sent LSA via Port" << port->getPortNumber();
    }
}

void Router::processLSA(const PacketPtr_t &packet, const PortPtr_t &incomingPort)
{
    if (!packet) return;

    QString payload = packet->getPayload();
    QStringList parts = payload.split(":");
    if (parts.size() < 3 || parts[0] != "LSA")
    {
        qWarning() << "Router" << m_id << "received malformed LSA packet.";
        return;
    }

    QString originIP = parts[1];
    QStringList linksList = parts[2].split(",");
    QVector<QString> links = QVector<QString>::fromList(linksList);

    int sequenceNumber = packet->getSequenceNumber();

    if (!m_lsdb.contains(originIP) || m_lsdb[originIP].sequenceNumber < sequenceNumber)
    {
        OSPFLSA newLSA;
        newLSA.originRouterIP = originIP;
        newLSA.links = links;
        newLSA.sequenceNumber = sequenceNumber;
        newLSA.age = 0;

        m_lsdb.insert(originIP, newLSA);
        qDebug() << "Router" << m_id << "updated LSDB with LSA from" << originIP;

        auto lsaPacket = QSharedPointer<Packet>::create(PacketType::OSPFLSA, payload, 10);
        lsaPacket->setSequenceNumber(sequenceNumber);

        for (const auto &port : m_ports)
        {
            if (!port->isConnected() || port == incomingPort) continue;

            if (port->getConnectedRouterIP().isEmpty()) continue;

            port->sendPacket(lsaPacket);
            qDebug() << "Router" << m_id << "flooded LSA via Port" << port->getPortNumber();
        }

        runDijkstra();
    }
    else
    {
        qDebug() << "Router" << m_id << "received outdated LSA from" << originIP << ". Ignoring.";
    }
}

void Router::runDijkstra()
{
    qDebug() << "Router" << m_id << "running Dijkstra algorithm.";

    m_distance.clear();
    m_previous.clear();
    QSet<QString> unvisited;

    for (auto &lsa : m_lsdb)
    {
        m_distance[lsa.originRouterIP] = INT32_MAX;
        unvisited.insert(lsa.originRouterIP);
    }

    m_distance[m_ipAddress->getIp()] = 0;

    while (!unvisited.isEmpty())
    {
        QString current;
        int minDistance = INT32_MAX;
        for (const auto &node : unvisited)
        {
            if (m_distance[node] < minDistance)
            {
                minDistance = m_distance[node];
                current = node;
            }
        }

        if (current.isEmpty() || minDistance == INT32_MAX)
            break;

        unvisited.remove(current);

        if (!m_lsdb.contains(current))
            continue;

        const auto &currentLSA = m_lsdb[current];
        for (const auto &neighborIP : currentLSA.links)
        {
            if (!m_distance.contains(neighborIP))
                continue;

            int altDistance = m_distance[current] + 1;
            if (altDistance < m_distance[neighborIP])
            {
                m_distance[neighborIP] = altDistance;
                m_previous[neighborIP] = current;
            }
        }
    }

    updateRoutingTable();
}

void Router::updateRoutingTable()
{
    qDebug() << "Router" << m_id << "updating routing table based on Dijkstra results.";

    for (int i = m_routingTable.size() - 1; i >= 0; i--)
    {
        if (m_routingTable[i].protocol == RoutingProtocol::OSPF && !m_routingTable[i].vip)
        {
            m_routingTable.removeAt(i);
        }
    }

    // Add new OSPF routes
    for (auto it = m_distance.constBegin(); it != m_distance.constEnd(); ++it)
    {
        const QString &dest = it.key();

        if (dest == m_ipAddress->getIp())
            continue;

        QString nextHop = "";
        QString current = dest;

        while (m_previous.contains(current))
        {
            if (m_previous[current] == m_ipAddress->getIp())
            {
                nextHop = current;
                break;
            }
            current = m_previous[current];
        }

        if (nextHop.isEmpty())
        {
            qDebug() << "Router" << m_id << "could not determine nextHop for destination" << dest;
            continue;
        }

        // Find the port connected to nextHop
        PortPtr_t outPort = nullptr;
        for (const auto &port : m_ports)
        {
            if (port->getConnectedRouterIP() == nextHop)
            {
                outPort = port;
                break;
            }
            else
            {
                QSharedPointer<PC> connectedPC = port->getConnectedPC();
                if (connectedPC && connectedPC->getIpAddress() == nextHop)
                {
                    outPort = port;
                    break;
                }
            }
        }

        if (outPort)
        {
            addRoute(dest, "255.255.255.255", nextHop, m_distance[dest], RoutingProtocol::OSPF, outPort);
            qDebug() << "Router" << m_id << "added OSPF route to" << dest << "via" << nextHop;

            static const QRegularExpression regex(R"(\.([a-zA-Z0-9_]+)$)");
            QString id = "";
            bool ok;

            QRegularExpressionMatch match1 = regex.match(dest);
            if (match1.hasMatch()) {
                id = match1.captured(1);
                qDebug() << "Extracted ID from Dest: " << id;
            } else {
            }

            QSharedPointer<Router> destRouter = s_topologyBuilder->findRouterById(id.toInt(&ok));
            std::vector<QSharedPointer<Router>> connectedPCs = destRouter->getDirectlyConnectedRouters(0, false);
            for (auto &pc : connectedPCs) {
                if (pc->getId() > 23) {
                    QString pcIP = pc->getIPAddress();
                    addRoute(pcIP, "255.255.255.255", nextHop, m_distance[dest] + 1, RoutingProtocol::OSPF, outPort);
                }
            }
        }
        else
        {
            qDebug() << "Router" << m_id << "could not find outPort for destination" << dest << "via" << nextHop;
        }
    }

    emit routingTableUpdated(m_id);
}

void Router::handleLSAExpiration()
{
    for (auto &lsa : m_lsdb)
    {
        lsa.age += 1;
    }

    QStringList expiredLSAs;
    for (auto it = m_lsdb.constBegin(); it != m_lsdb.constEnd(); ++it)
    {
        if (it.value().age >= OSPF_LSA_AGE_LIMIT)
        {
            expiredLSAs.append(it.key());
        }
    }

    for (const auto &originIP : expiredLSAs)
    {
        m_lsdb.remove(originIP);
        qDebug() << "Router" << m_id << "removed expired LSA from" << originIP;
        runDijkstra();
    }
}

bool Router::isRouterBorder() {
    bool isBorder = false;
    for (auto &port : m_ports) {
        if (m_ASnum != -1) {
            Range range = getRange(m_ASnum);
            int connectedRouterId = port->getConnectedRouterId();
            if (connectedRouterId != -1) {
                if (connectedRouterId <= range.pcMax && connectedRouterId >= range.pcMin) {
                } else if (connectedRouterId > range.max || connectedRouterId < range.min) {
                    isBorder = true;
                }
            }
        }
    }
    return isBorder;
}

void Router::startEBGP() {
    for (auto &port : m_ports) {
        if (m_ASnum != -1) {
            Range range = getRange(m_ASnum);
            int connectedRouterId = port->getConnectedRouterId();
            if (connectedRouterId != -1) {
                if (connectedRouterId <= range.pcMax && connectedRouterId >= range.pcMin) {
                } else if (connectedRouterId > range.max || connectedRouterId < range.min) {
                    if (!port->isConnected()) continue;

                    QString payload = "EBGP_UPDATE:";
                    int routeCount = 0;
                    for (const auto &entry : m_routingTable) {
                        int advertisedMetric = entry.metric;
                        if (entry.learnedFromPort == port && !entry.isDirect) {
                            advertisedMetric = RIP_INFINITY;
                        }
                        payload += entry.destination + "," + entry.mask + "," + QString::number(advertisedMetric) + "#";
                        routeCount++;
                    }

                    payload += m_ipAddress->getIp();
                    if (routeCount == 0) {
                        payload = "EBGP_UPDATE:" + m_ipAddress->getIp();
                    }

                    auto updatePacket = QSharedPointer<Packet>::create(PacketType::Control, payload);
                    updatePacket->setTTL(10);
                    port->sendPacket(updatePacket);
                    qDebug() << "Router" << m_id << "sent EBGP update via Port" << port->getPortNumber() << "with" << routeCount << "routes";
                }
            }
        }
    }
}

void Router::startIBGP() {
    IBGPCounter = 0;
    for (auto &port : m_ports) {
        if (m_ASnum != -1) {
            Range range = getRange(m_ASnum);
            int connectedRouterId = port->getConnectedRouterId();
            if (connectedRouterId != -1) {
                QSharedPointer<Router> foundRouter = RouterRegistry::findRouterById(connectedRouterId);
                if (connectedRouterId <= range.pcMax && connectedRouterId >= range.pcMin) {
                } else if (connectedRouterId <= range.max && connectedRouterId >= range.min && !foundRouter->isRouterBorder()) {
                    if (!port->isConnected()) continue;

                    QString payload = "IBGP_UPDATE:";
                    int routeCount = 0;
                    QString destination = (m_ASnum == 1) ? "192.168.200.xx" : "192.168.100.xx";
                    payload += destination + "," + "255.255.255.255" + "," + QString::number(1) + "#";
                    routeCount++;

                    payload += m_ipAddress->getIp();
                    if (routeCount == 0) {
                        payload = "IBGP_UPDATE:" + m_ipAddress->getIp();
                    }

                    auto updatePacket = QSharedPointer<Packet>::create(PacketType::Control, payload);
                    updatePacket->setTTL(10);
                    port->sendPacket(updatePacket);
                    qDebug() << "Router" << m_id << "sent IBGP update via Port" << port->getPortNumber() << "with" << routeCount << "routes";
                }
            }
        }
    }
}

void Router::processEBGPUpdate(const PacketPtr_t &packet)
{
    if (!packet) return;

    QString payload = packet->getPayload();

    auto parts = payload.split(":");
    if (parts.size() < 2) {
        qWarning() << "Router" << m_id << "received malformed EBGP update:" << payload;
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

    for (const auto &routeStr : routesAndSender) {
        if (routeStr.isEmpty()) continue;
        auto fields = routeStr.split(",");
        if (fields.size() < 3) {
            qWarning() << "Router" << m_id << "EBGP route entry malformed:" << routeStr;
            continue;
        }

        QString dest = fields[0];
        QString mask = fields[1];
        int recvMetric = fields[2].toInt();
        int newMetric = recvMetric + 1;

        if (newMetric >= RIP_INFINITY) {
            qDebug() << "Router" << m_id << "received unreachable route for" << dest << "skipping.";
            continue;
        }

        addRoute(dest, mask, senderIP, newMetric, RoutingProtocol::EBGP, incomingPortPtr);
    }
}

void Router::processIBGPUpdate(const PacketPtr_t &packet)
{
    m_gotIBGP = true;

    if (!packet) return;

    QString payload = packet->getPayload();

    auto parts = payload.split(":");
    if (parts.size() < 2) {
        qWarning() << "Router" << m_id << "received malformed IBGP update:" << payload;
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

    for (const auto &routeStr : routesAndSender) {
        if (routeStr.isEmpty()) continue;
        auto fields = routeStr.split(",");
        if (fields.size() < 3) {
            qWarning() << "Router" << m_id << "IBGP route entry malformed:" << routeStr;
            continue;
        }

        QString dest = fields[0];
        QString mask = fields[1];

        int recvMetric = fields[2].toInt();
        int newMetric = recvMetric + 1;

        if (newMetric >= RIP_INFINITY) {
            qDebug() << "Router" << m_id << "received unreachable route for" << dest << "skipping.";
            continue;
        }

        addRoute(dest, mask, senderIP, newMetric, RoutingProtocol::IBGP, incomingPortPtr);
    }

    if (IBGPCounter < 10) {
        forwardIBGP();
    }
}

void Router::forwardIBGP() {
    for (auto &port : m_ports) {
        if (m_ASnum != -1) {
            Range range = getRange(m_ASnum);
            int connectedRouterId = port->getConnectedRouterId();
            if (connectedRouterId != -1) {
                QSharedPointer<Router> foundRouter = RouterRegistry::findRouterById(connectedRouterId);
                if (connectedRouterId <= range.pcMax && connectedRouterId >= range.pcMin) {
                } else if (connectedRouterId <= range.max && connectedRouterId >= range.min && !foundRouter->isRouterBorder()) {
                    if (!port->isConnected()) continue;
                    QString destination = (m_ASnum == 1) ? "192.168.200.xx" : "192.168.100.xx";

                    QString payload = "IBGP_UPDATE:";
                    int routeCount = 0;
                    for (const auto &entry : m_routingTable) {
                        if (entry.destination == destination) {
                            int advertisedMetric = entry.metric;
                            if (entry.learnedFromPort == port && !entry.isDirect) {
                                advertisedMetric = RIP_INFINITY;
                            }
                            payload += entry.destination + "," + entry.mask + "," + QString::number(advertisedMetric) + "#";
                            routeCount++;
                        }
                    }

                    payload += m_ipAddress->getIp();
                    if (routeCount == 0) {
                        payload = "IBGP_UPDATE:" + m_ipAddress->getIp();
                    }

                    auto updatePacket = QSharedPointer<Packet>::create(PacketType::Control, payload);
                    updatePacket->setTTL(10);
                    port->sendPacket(updatePacket);
                    qDebug() << "Router" << m_id << "sent IBGP update via Port" << port->getPortNumber() << "with" << routeCount << "routes";
                }
            }
        }
    }
}
