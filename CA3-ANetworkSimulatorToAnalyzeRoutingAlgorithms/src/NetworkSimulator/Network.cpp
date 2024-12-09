#include "Network.h"
#include <QDebug>

Network::Network(const QJsonObject &config, QObject *parent)
    : QObject(parent), m_config(config)
{
}

Network::~Network()
{
    // Cleanup if necessary
}

void Network::initialize()
{
    createAutonomousSystems();

    for (auto &asInstance : m_autonomousSystems)
    {
        asInstance->createRouters();
    }

    for (auto &asInstance : m_autonomousSystems)
    {
        asInstance->createPCs();
    }

    for (auto &asInstance : m_autonomousSystems)
    {
        asInstance->setupTopology();
    }

    connectAutonomousSystems();
}


void Network::createAutonomousSystems()
{
    QJsonArray asArray = m_config.value("Autonomous_systems").toArray();
    for (const QJsonValue &asValue : asArray)
    {
        QJsonObject asObject = asValue.toObject();
        auto asInstance = QSharedPointer<AutonomousSystem>::create(asObject);
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

std::vector<QSharedPointer<AutonomousSystem>> Network::getAutonomousSystems() const
{
    return m_autonomousSystems;
}
