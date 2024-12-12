#include "Network.h"
#include "Topology/TopologyController.h"
#include <QDebug>
#include <QJsonArray>

Network::Network(const QJsonObject &config, QObject *parent)
    : QObject(parent), m_config(config)
{
}

Network::~Network()
{
    // Cleanup if necessary
}

void Network::initialize(const IdAssignment &idAssignment)
{
    createAutonomousSystems(idAssignment);
    connectAutonomousSystems();
}

void Network::createAutonomousSystems(const IdAssignment &idAssignment)
{
    QJsonArray asArray = m_config.value("Autonomous_systems").toArray();
    for (const QJsonValue &asValue : asArray)
    {
        QJsonObject asObject = asValue.toObject();
        auto asInstance = QSharedPointer<AutonomousSystem>::create(asObject, idAssignment);
        m_autonomousSystems.push_back(asInstance);
    }
}

void Network::connectAutonomousSystems()
{
    for (const auto &asInstance : m_autonomousSystems)
    {
        asInstance->connectToOtherAS(m_autonomousSystems);
    }
}

void Network::initiateDHCPPhase()
{
    for (const auto &asInstance : m_autonomousSystems)
    {
        auto topologyController = asInstance->getTopologyController();
        if (topologyController)
        {
            topologyController->initiateDHCPPhase();
        }
        else
        {
            qWarning() << "TopologyController is not initialized for AS.";
        }
    }
}
