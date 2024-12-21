#include "Simulator.h"
#include "EventsCoordinator/EventsCoordinator.h"
#include <QFile>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>

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

    preAssignIDs();

    return true;
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
    m_network->initialize(m_idAssignment);

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

    m_metricsCollector = QSharedPointer<MetricsCollector>::create();

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

    // Now we know all routers have IP addresses assigned, so we can setup direct routes:
    if (m_network) {
        m_network->setupDirectRoutesForRouters();
        m_network->finalizeRoutesAfterDHCP();
    }

    // Start the event coordinator clock so RIP ticks can begin
    EventsCoordinator::instance()->startClock(std::chrono::milliseconds(100));

    // Enable RIP on all routers
    if (m_network) {
        m_network->enableRIPOnAllRouters();
    }
}

void Simulator::onConvergenceDetected()
{
    qDebug() << "Convergence detected. Printing all routing tables:";
    if (m_network) {
        m_network->printAllRoutingTables();
    }

    qDebug() << "Proceeding with further steps.";

    auto eventsCoordinator = EventsCoordinator::instance();
    eventsCoordinator->quit();
    eventsCoordinator->wait();

    qDebug() << "EventsCoordinator stopped. Ready for next actions.";

    // Now we can proceed with sending packets or other operations
    initiatePacketSending();

    // Print metrics after some delay to allow packet processing
    QTimer::singleShot(5000, this, [this]() {
        if (m_metricsCollector) {
            m_metricsCollector->printStatistics();
        }
    });
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
