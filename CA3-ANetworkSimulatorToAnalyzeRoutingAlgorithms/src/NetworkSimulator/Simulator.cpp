#include "Simulator.h"
#include <QFile>
#include <QJsonDocument>
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
}

void Simulator::startSimulation()
{
    qDebug() << "Simulation initialized. Network topology is set up.";

    // Initiate DHCP Phase
    initiateDHCPPhase();

    QThread::sleep(5);

    // Check the assigned IP's
    checkAssignedIP();

    // Start clock, events, etc. Complete In Future Phases
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
