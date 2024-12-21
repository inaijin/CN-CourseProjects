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
    }

    connect(eventsCoordinator, &EventsCoordinator::convergenceDetected, this, &Simulator::onConvergenceDetected);
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

    // Now you can proceed with sending packets or other operations
    initiatePacketSending();
}

void Simulator::initiatePacketSending()
{
    qDebug() << "Initiating packet sending based on updated routing tables.";
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
