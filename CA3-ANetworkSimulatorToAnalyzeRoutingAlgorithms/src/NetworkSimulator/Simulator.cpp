#include "Simulator.h"
#include "EventsCoordinator/EventsCoordinator.h"
#include <QFile>
#include <iostream>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>
#include <QRegularExpression>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <RouterRegistry.h>

Simulator::Simulator(QObject *parent)
    : QObject(parent)
{
}

Simulator::~Simulator()
{
    // Cleanup if necessary
}

bool Simulator::loadConfig(const QString &configFilePath)
{
    QFile configFile(configFilePath);
    if (!configFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open config file:" << configFilePath;
        return false;
    }

    QByteArray data = configFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
    {
        qWarning() << "Failed to parse config file.";
        return false;
    }

    m_config = doc.object();

    QString cycleDurationStr = m_config.value("cycle_duration").toString("100ms");
    m_cycleDuration = parseDuration(cycleDurationStr);

    preAssignIDs();

    return true;
}

std::chrono::milliseconds Simulator::parseDuration(const QString &durationStr)
{
    static const QRegularExpression regex(R"((\d+)(ms|s|min|h))");
    QRegularExpressionMatch match = regex.match(durationStr.trimmed());

    if (!match.hasMatch()) {
        qWarning() << "Invalid duration format:" << durationStr << ". Using default 100ms.";
        return std::chrono::milliseconds(100);
    }

    int value = match.captured(1).toInt();
    QString unit = match.captured(2);

    if (unit == "ms") {
        return std::chrono::milliseconds(value);
    } else if (unit == "s") {
        return std::chrono::milliseconds(value * 1000);
    } else if (unit == "min") {
        return std::chrono::milliseconds(value * 60 * 1000);
    } else if (unit == "h") {
        return std::chrono::milliseconds(value * 60 * 60 * 1000);
    } else {
        qWarning() << "Unknown time unit in duration:" << unit << ". Using default 100ms.";
        return std::chrono::milliseconds(100);
    }
}

void Simulator::preAssignIDs()
{
    QJsonArray asArray = m_config.value("Autonomous_systems").toArray();

    int currentMaxId = 0;

    int totalNodeCount = 0;
    for (const QJsonValue &asVal : asArray)
    {
        QJsonObject asObj = asVal.toObject();
        int nodeCount = asObj.value("node_count").toInt();
        totalNodeCount += nodeCount;
    }

    for (const QJsonValue &asVal : asArray)
    {
        QJsonObject asObj = asVal.toObject();
        int asId = asObj.value("id").toInt();
        int nodeCount = asObj.value("node_count").toInt();
        QJsonArray gateways = asObj.value("gateways").toArray();

        int pcCount = 0;
        for (const QJsonValue &gwVal : gateways)
        {
            QJsonObject gwObj = gwVal.toObject();
            QJsonArray users = gwObj.value("users").toArray();
            pcCount += users.size();
        }

        int routerStartId = currentMaxId + 1;
        int routerEndId = currentMaxId + nodeCount;
        int pcStartId = totalNodeCount + 1;
        int pcEndId = pcStartId + pcCount - 1;

        currentMaxId = routerEndId;
        totalNodeCount += pcCount;

        m_idAssignment.addAsIdRange(asId, routerStartId, routerEndId, pcStartId, pcEndId);
    }

    qDebug() << "ID pre-assignment done.";
}

void Simulator::initializeNetwork()
{
    m_network = QSharedPointer<Network>::create(m_config);
    m_network->initialize(m_idAssignment, addTorus);

    m_metricsCollector = QSharedPointer<MetricsCollector>::create();

    auto allRouters = m_network->getAllRouters();
    auto eventsCoordinator = EventsCoordinator::instance();
    for (const auto &router : allRouters) {
        eventsCoordinator->addRouter(router);
        router->initialize();
        router->setMetricsCollector(m_metricsCollector);
    }

    connect(eventsCoordinator, &EventsCoordinator::convergenceDetected, this, &Simulator::onConvergenceDetected);

    m_dataGenerator = QSharedPointer<DataGenerator>::create();
    m_dataGenerator->setLambda(1.0);

    std::vector<QSharedPointer<PC>> allPCs;
    for (const auto &asInstance : m_network->getAutonomousSystems()) {
        auto pcs = asInstance->getPCs();
        allPCs.insert(allPCs.end(), pcs.begin(), pcs.end());
    }

    m_dataGenerator->setSenders(allPCs);

    for (const auto &pc : allPCs) {
        pc->setMetricsCollector(m_metricsCollector);
    }

    connect(m_dataGenerator.data(), &DataGenerator::packetsGenerated, this, &Simulator::handleGeneratedPackets);
}

void Simulator::handleGeneratedPackets(const std::vector<QSharedPointer<Packet>> &packets)
{
    qDebug() << "Simulator received" << packets.size() << "generated packets.";

    for (const auto &packet : packets) {
        if (packet->getPath().size() < 2) {
            qWarning() << "Simulator: Packet" << packet->getId() << "has insufficient path information.";
            m_metricsCollector->recordPacketDropped();
            continue;
        }

        QString senderIP = packet->getPath().at(0);
        QString destinationIP = packet->getPath().at(1);

        QSharedPointer<PC> sender = nullptr;
        for (const auto &pc : m_dataGenerator->getSenders()) {
            if (pc->getIpAddress() == senderIP) {
                sender = pc;
                break;
            }
        }

        if (!sender) {
            qWarning() << "Simulator: Sender PC with IP" << senderIP << "not found.";
            m_metricsCollector->recordPacketDropped();
            continue;
        }

        auto port = sender->getPort();
        if (port) {
            port->sendPacket(packet);
            qDebug() << "Simulator: Packet" << packet->getId() << "sent from PC" << sender->getId() << "to" << destinationIP;
            m_metricsCollector->recordPacketSent();
        } else {
            qWarning() << "Simulator: Sender PC" << sender->getId() << "has no available port.";
            m_metricsCollector->recordPacketDropped();
        }
    }
}

void Simulator::startSimulation()
{
    RoutingProtocol protocolAS1 = (firstASAlgo == 1) ? RoutingProtocol::RIP : RoutingProtocol::OSPF;
    RoutingProtocol protocolAS2 = (secondASAlgo == 1) ? RoutingProtocol::RIP : RoutingProtocol::OSPF;

    RoutingProtocol protocol = (mainAlgo == 1) ? RoutingProtocol::RIP : RoutingProtocol::OSPF;
    qDebug() << "Simulation initialized. Network topology is set up.";

    // Initiate DHCP Phase for routers
    initiateDHCPPhase();
    QThread::sleep(5);

    // Initiate DHCP Phase for PCs
    if (m_network) {
        m_network->initiateDHCPPhaseForPC();
    }
    QThread::sleep(5);

    // Check the assigned IP's
    checkAssignedIP();

    // Check the assigned IP's for PCs
    checkAssignedIPPC();

    printTopologyVisualization();

    // Now we know all routers have IP addresses assigned, so we can setup direct routes:
    if (m_network) {
        m_network->setupDirectRoutesForRouters(protocol);
        m_network->finalizeRoutesAfterDHCP(protocol, useBGP, protocolAS1, protocolAS2);
    }

    // Start the event coordinator clock so RIP ticks can begin
    EventsCoordinator::instance()->startClock(m_cycleDuration);

    // Enable RIP on all routers
    if (m_network) {
        if (useBGP) {
            m_network->startBGP(protocolAS1, protocolAS2);
        } else if (protocol == RoutingProtocol::RIP) {
            m_network->enableRIPOnAllRouters();
        } else {
            m_network->enableOSPFOnAllRouters();
        }
    }
}

void Simulator::onConvergenceDetected()
{
    qDebug() << "Convergence detected. Preparing to print all routing tables:";

    auto executeConvergenceActions = [this]() {
        if (m_network) {
            m_network->printAllRoutingTables();
        }

        qDebug() << "Proceeding with further steps.";

        auto eventsCoordinator = EventsCoordinator::instance();
        eventsCoordinator->quit();
        eventsCoordinator->wait();

        qDebug() << "EventsCoordinator stopped. Ready for next actions.";

        initiatePacketSending();

        QTimer::singleShot(5000, this, [this]() {
            if (m_metricsCollector) {
                m_metricsCollector->printStatistics();
            }
        });
    };

    if (useBGP) {
        qDebug() << "BGP is enabled. Delaying execution of convergence actions.";
        m_network->startEBGP();
        QTimer::singleShot(3000, this, [this]() {
            m_network->startIBGP();
        });
        QTimer::singleShot(5000, this, executeConvergenceActions);
    }
    else {
        executeConvergenceActions();
        qDebug() << "BGP is not enabled. Executing convergence actions immediately.";
    }
}

void Simulator::initiatePacketSending()
{
    qDebug() << "Initiating packet sending based on updated routing tables.";

    if (m_dataGenerator) {
        m_dataGenerator->generatePackets();
        qDebug() << "DataGenerator has been instructed to generate packets.";
    } else {
        qWarning() << "DataGenerator not initialized. Cannot generate packets.";
    }
}

void Simulator::initiateDHCPPhase()
{
    if (m_network) {
        m_network->initiateDHCPPhase();
    } else {
        qWarning() << "Failed to initiate DHCP phase: Network not initialized.";
    }
}

void Simulator::checkAssignedIP() {
    if (m_network) {
        m_network->checkAssignedIP();
    } else {
        qWarning() << "Failed to check IP: Network not initialized.";
    }
}

void Simulator::checkAssignedIPPC() {
    if (m_network) {
        m_network->checkAssignedIPPC();
    } else {
        qWarning() << "Failed to check IP: Network not initialized.";
    }
}

bool Simulator::configureFromCommandLine(const QStringList& arguments)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("UT Network Simulator");
    parser.addHelpOption();

    QCommandLineOption bgpOption(QStringList() << "b" << "bgp",
                                 "Enable BGP (yes/no).",
                                 "bgp");
    QCommandLineOption firstASAlgoOption(QStringList() << "f" << "first-as-algo",
                                         "First AS algorithm (1 for RIP, 2 for OSPF).",
                                         "first-as-algo");
    QCommandLineOption secondASAlgoOption(QStringList() << "s" << "second-as-algo",
                                          "Second AS algorithm (1 for RIP, 2 for OSPF).",
                                          "second-as-algo");
    QCommandLineOption mainAlgoOption(QStringList() << "m" << "main-algo",
                                      "Main routing algorithm (1 for RIP, 2 for OSPF).",
                                      "main-algo");
    QCommandLineOption torusOption(QStringList() << "t" << "torus",
                                   "Add torus topology (yes/no).",
                                   "torus");

    parser.addOption(bgpOption);
    parser.addOption(firstASAlgoOption);
    parser.addOption(secondASAlgoOption);
    parser.addOption(mainAlgoOption);
    parser.addOption(torusOption);

    parser.process(arguments);

    bool argumentsProvided = false;

    if (parser.isSet(bgpOption) || parser.isSet(firstASAlgoOption) ||
       parser.isSet(secondASAlgoOption) || parser.isSet(mainAlgoOption) ||
       parser.isSet(torusOption)) {
        argumentsProvided = true;

        if (parser.isSet(bgpOption)) {
            QString bgpValue = parser.value(bgpOption).toLower();
            if (bgpValue == "yes" || bgpValue == "y") {
                setUseBGP(true);
            } else if (bgpValue == "no" || bgpValue == "n") {
                setUseBGP(false);
            } else {
                qWarning() << "Invalid value for BGP option. Expected 'yes' or 'no'. Using default 'no'.";
                setUseBGP(false);
            }
        } else {
            setUseBGP(false);
        }

        if (useBGP) {
            if (parser.isSet(firstASAlgoOption)) {
                bool ok;
                int algo = parser.value(firstASAlgoOption).toInt(&ok);
                if (ok && (algo == 1 || algo == 2)) {
                    setFirstASAlgo(algo);
                } else {
                    qWarning() << "Invalid value for first AS algorithm. Expected 1 or 2. Using default 0.";
                    setFirstASAlgo(0);
                }
            } else {
                qWarning() << "First AS algorithm not provided. Using default 0.";
                setFirstASAlgo(0);
            }

            if (parser.isSet(secondASAlgoOption)) {
                bool ok;
                int algo = parser.value(secondASAlgoOption).toInt(&ok);
                if (ok && (algo == 1 || algo == 2)) {
                    setSecondASAlgo(algo);
                } else {
                    qWarning() << "Invalid value for second AS algorithm. Expected 1 or 2. Using default 0.";
                    setSecondASAlgo(0);
                }
            } else {
                qWarning() << "Second AS algorithm not provided. Using default 0.";
                setSecondASAlgo(0);
            }
        } else {
            if (parser.isSet(mainAlgoOption)) {
                bool ok;
                int algo = parser.value(mainAlgoOption).toInt(&ok);
                if (ok && (algo == 1 || algo == 2)) {
                    setMainAlgo(algo);
                } else {
                    qWarning() << "Invalid value for main algorithm. Expected 1 or 2. Using default 0.";
                    setMainAlgo(0);
                }
            } else {
                qWarning() << "Main routing algorithm not provided. Using default 0.";
                setMainAlgo(0);
            }
        }

        if (parser.isSet(torusOption)) {
            QString torusValue = parser.value(torusOption).toLower();
            if (torusValue == "yes" || torusValue == "y") {
                setAddTorus(true);
            } else if (torusValue == "no" || torusValue == "n") {
                setAddTorus(false);
            } else {
                qWarning() << "Invalid value for torus option. Expected 'yes' or 'no'. Using default 'no'.";
                setAddTorus(false);
            }
        } else {
            setAddTorus(false);
        }
    }

    if (!argumentsProvided) {
        std::cout << "Welcome To Our UT Network Simulator !!!" << std::endl;

        bool bgp = promptYesNo("Do you want BGP? (yes/no): ");
        setUseBGP(bgp);

        if (bgp) {
            int algo1 = promptForAlgorithm("Enter the first AS algorithm (1 for RIP, 2 for OSPF): ");
            setFirstASAlgo(algo1);

            int algo2 = promptForAlgorithm("Enter the second AS algorithm (1 for RIP, 2 for OSPF): ");
            setSecondASAlgo(algo2);
        } else {
            int mainAlgoInput = promptForAlgorithm("Enter the main routing algorithm you want (1 for RIP, 2 for OSPF): ");
            setMainAlgo(mainAlgoInput);
        }

        bool torus = promptYesNo("Do you want to add a torus topology? (yes/no): ");
        setAddTorus(torus);
    }

    return true;
}

bool Simulator::promptYesNo(const QString& prompt)
{
    std::string input;
    while (true) {
        std::cout << prompt.toStdString();
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::string lowerInput = input;
        for (auto & c: lowerInput) c = tolower(c);
        if (lowerInput == "yes" || lowerInput == "y") {
            return true;
        } else if (lowerInput == "no" || lowerInput == "n") {
            return false;
        } else {
            std::cerr << "Please enter 'yes' or 'no'." << std::endl;
        }
    }
}

int Simulator::promptForAlgorithm(const QString& prompt)
{
    std::string input;
    while (true) {
        std::cout << prompt.toStdString();
        std::getline(std::cin, input);
        try {
            int algo = std::stoi(input);
            if (algo == 1 || algo == 2) {
                return algo;
            } else {
                std::cerr << "Invalid input. Please enter 1 for RIP or 2 for OSPF." << std::endl;
            }
        } catch (...) {
            std::cerr << "Invalid input. Please enter a number (1 for RIP, 2 for OSPF)." << std::endl;
        }
    }
}

void Simulator::setUseBGP(bool bgp) { useBGP = bgp; }
void Simulator::setFirstASAlgo(int algo) { firstASAlgo = algo; }
void Simulator::setSecondASAlgo(int algo) { secondASAlgo = algo; }
void Simulator::setMainAlgo(int algo) { mainAlgo = algo; }
void Simulator::setAddTorus(bool torus) { addTorus = torus; }

void Simulator::printTopologyVisualization()
{
    auto asList = m_network->getAutonomousSystems();

    std::cout << "\n=============== NETWORK TOPOLOGY VISUALIZATION ===============\n";

    for (const auto &asInstance : asList)
    {
        int asId = asInstance->getId();
        std::cout << "Autonomous System: AS" << asId << "\n";

        // --- Print routers ---
        auto routers = asInstance->getRouters();
        if (!routers.empty())
        {
            std::cout << "  -- Routers --\n";
            for (const auto &router : routers)
            {
                int rid      = router->getId();
                QString ip   = router->getIPAddress();
                bool broken  = router->isBroken();

                std::cout << "     * Router ID: " << rid
                          << ", IP: " << ip.toStdString();

                // Indicate broken routers
                if (broken)
                    std::cout << " [BROKEN!]";

                std::cout << "\n";
            }
        }

        // --- Print PCs ---
        auto pcs = asInstance->getPCs();
        if (!pcs.empty())
        {
            std::cout << "  -- PCs --\n";
            for (const auto &pc : pcs)
            {
                int pid    = pc->getId();
                QString ip = pc->getIpAddress();

                std::cout << "     - PC ID: " << pid
                          << ", IP: " << ip.toStdString() << "\n";
            }
        }

        std::cout << "\n";
    }

    std::cout << "==============================================================\n\n";

    std::cout << std::flush;

    printAsciiDiagram(addTorus);
}

static inline void printRouter(int id, bool broken)
{
    if (broken)
        std::cout << "(X)" << id;
    else
        std::cout << "(R)" << id;
}

static inline void printPC(int id)
{
    std::cout << "[P]" << id;
}

void Simulator::printAsciiDiagram(bool addTorus)
{
    std::cout << "========= DETAILED ASCII TOPOLOGY DIAGRAM =========\n\n";

    auto asList = m_network->getAutonomousSystems();

    for (auto &asInstance : asList)
    {
        int asId = asInstance->getId();

        // Print the AS label
        std::cout << "   +------------------------------------+\n";
        std::cout << "   |       Autonomous System AS" << asId << "        |\n";
        std::cout << "   +------------------------------------+\n\n";

        if (asId == 1)
        {
            //--- AS1 Diagram ---
            if (addTorus)
            {
                //--- The standard 4×4 mesh with PCs across the top plus the torus addition. ---
                auto r = [&](int rid){
                    auto router = RouterRegistry::findRouterById(rid);
                    if (router)
                        printRouter(rid, router->isBroken());
                    else
                        std::cout << "(?)" << rid;
                };
                auto p = [&](int pcid){ printPC(pcid); };

                // Top row of PCs
                std::cout << "      ";
                p(24); std::cout << "  "; p(25); std::cout << "   ";
                p(26); std::cout << "  "; p(27); std::cout << "   ";
                p(28); std::cout << "  "; p(29); std::cout << "   ";
                p(30); std::cout << "  "; p(31);
                std::cout << "\n";
                std::cout << "          ^               ^               ^               ^  \n";
                std::cout << "        \\ | /           \\ | /           \\ | /           \\ | /\n";

                // First router row
                std::cout << "       (-"; r(1);  std::cout << " ---------- "; r(2);
                std::cout << " ---------- "; r(3);  std::cout << " --------- "; r(4);  std::cout << "-)\n";

                // Second row
                std::cout << "          |               |               |               |\n";
                std::cout << "       (-"; r(5);  std::cout << " ---------- "; r(6);  std::cout << " ---------- "; r(7);  std::cout << " --------- "; r(8);  std::cout << "-)\n";

                // Third row
                std::cout << "          |               |               |               |\n";
                std::cout << "       (-"; r(9);  std::cout << " --------- ";  r(10); std::cout << " --------- ";  r(11); std::cout << " -------- ";  r(12); std::cout << "-)\n";

                // Fourth row
                std::cout << "          |               |               |               |\n";
                std::cout << "       (-"; r(13); std::cout << " -------- "; r(14); std::cout << " --------- "; r(15); std::cout << " -------- "; r(16);
                std::cout << "-)\n";
                std::cout << "          |               |               |               |\n";
                std::cout << "          v               v               v               v\n\n";
            }
            else
            {
                //--- The standard 4×4 mesh with PCs across the top. ---
                auto r = [&](int rid){
                    auto router = RouterRegistry::findRouterById(rid);
                    if (router)
                        printRouter(rid, router->isBroken());
                    else
                        std::cout << "(?)" << rid;
                };
                auto p = [&](int pcid){ printPC(pcid); };

                // Top row of PCs
                std::cout << "      ";
                p(24); std::cout << "  "; p(25); std::cout << "   ";
                p(26); std::cout << "  "; p(27); std::cout << "   ";
                p(28); std::cout << "  "; p(29); std::cout << "   ";
                p(30); std::cout << "  "; p(31);
                std::cout << "\n";
                std::cout << "        \\   /           \\   /           \\   /           \\   /\n";

                // First router row
                std::cout << "         "; r(1);  std::cout << " ---------- "; r(2);
                std::cout << " ---------- "; r(3);  std::cout << " --------- "; r(4);  std::cout << "\n";

                // Second row
                std::cout << "          |               |               |               |\n";
                std::cout << "         "; r(5);  std::cout << " ---------- "; r(6);  std::cout << " ---------- "; r(7);  std::cout << " --------- "; r(8);  std::cout << "\n";

                // Third row
                std::cout << "          |               |               |               |\n";
                std::cout << "         "; r(9);  std::cout << " --------- ";  r(10); std::cout << " --------- ";  r(11); std::cout << " -------- ";  r(12); std::cout << "\n";

                // Fourth row
                std::cout << "          |               |               |               |\n";
                std::cout << "         "; r(13); std::cout << " -------- "; r(14); std::cout << " --------- "; r(15); std::cout << " -------- "; r(16);
                std::cout << "\n\n";
            }
        }
        else if (asId == 2)
        {
            auto r = [&](int rid){
                auto router = RouterRegistry::findRouterById(rid);
                if (router)
                    printRouter(rid, router->isBroken());
                else
                    std::cout << "(?)" << rid;
            };
            auto p = [&](int pcid){ printPC(pcid); };

            // Top row
            std::cout << "         "; r(17);  std::cout << " ---------- "; r(18);
            std::cout << " ---------- "; r(19); std::cout << "\n";

            // Middle row
            std::cout << "          |              \\     /            |                \n";
            std::cout << "  "; p(37); std::cout<< "-"; r(22);  std::cout << "            "; r(23);  std::cout << "            "; r(20); std::cout << "-"; p(34); p(35);  std::cout << "\n";

            std::cout << "          |        \\        |        /       |        \n";
            std::cout << "       "; p(38); std::cout << " "; std::cout << "             "; r(21);  std::cout << "            "; p(36); std::cout << "\n";

            std::cout << "                           /  \\  \n";
            std::cout << "                      "; p(32); std::cout << "    "; p(33);
            std::cout << "\n\n";
        }
    }

    std::cout << "===========================================================\n\n" << std::flush;
}
