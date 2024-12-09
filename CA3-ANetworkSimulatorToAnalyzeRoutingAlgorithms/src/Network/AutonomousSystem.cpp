#include "AutonomousSystem.h"
#include "../Topology/TopologyBuilder.h"
#include "../Topology/TopologyController.h"
#include <QDebug>

AutonomousSystem::AutonomousSystem(const QJsonObject &config, QObject *parent)
    : QObject(parent), m_config(config)
{
    if (!config.contains("id"))
    {
        qWarning() << "Invalid configuration for AutonomousSystem. Missing required ID field.";
        return;
    }

    m_id = config.value("id").toInt();

    m_topologyBuilder = QSharedPointer<TopologyBuilder>::create(m_config, this);
    m_topologyBuilder->buildTopology();

    m_topologyController = QSharedPointer<TopologyController>::create(m_topologyBuilder, this);

    qDebug() << "AutonomousSystem initialized with ID:" << m_id;
}

AutonomousSystem::~AutonomousSystem() {}

int AutonomousSystem::getId() const
{
    return m_id;
}

const std::vector<QSharedPointer<Router>> &AutonomousSystem::getRouters() const
{
    return m_topologyBuilder->getRouters();
}

void AutonomousSystem::connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS)
{
    m_topologyController->connectToOtherAS(allAS);
}
