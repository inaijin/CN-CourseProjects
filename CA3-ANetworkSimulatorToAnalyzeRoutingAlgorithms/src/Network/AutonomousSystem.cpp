#include "AutonomousSystem.h"
#include "../Topology/TopologyBuilder.h"
#include "../Topology/TopologyController.h"
#include <QDebug>

AutonomousSystem::AutonomousSystem(const QJsonObject &config, QObject *parent)
    : QObject(parent), m_config(config)
{
    if (!config.contains("id") || !config.contains("topology_type"))
    {
        qWarning() << "Invalid configuration for AutonomousSystem. Missing required fields.";
        return;
    }

    m_id = config.value("id").toInt();
    m_topologyType = config.value("topology_type").toString();

    if (m_id < 1)
    {
        qWarning() << "Invalid AutonomousSystem ID. Must be positive.";
        return;
    }

    m_topologyBuilder = QSharedPointer<TopologyBuilder>::create(m_config, this);
    m_topologyBuilder->buildTopology();

    m_topologyController = QSharedPointer<TopologyController>::create(m_topologyBuilder, this);

    qDebug() << "AutonomousSystem initialized with ID:" << m_id;
}

AutonomousSystem::~AutonomousSystem()
{
}

int AutonomousSystem::getId() const
{
    return m_id;
}

void AutonomousSystem::connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS)
{
    m_topologyController->connectToOtherAS(allAS);
}
