#include <QDebug>

#include "../Network/PC.h"
#include "AutonomousSystem.h"
#include "../Network/Router.h"
#include "../Topology/TopologyBuilder.h"
#include "../Topology/TopologyController.h"

AutonomousSystem::AutonomousSystem(const QJsonObject &config, const IdAssignment &idAssignment, const bool torus, QObject *parent)
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

    m_topologyBuilder = QSharedPointer<TopologyBuilder>::create(m_config, idAssignment, this);
    m_topologyBuilder->buildTopology(torus);

    m_topologyController = QSharedPointer<TopologyController>::create(m_topologyBuilder, this);

    qDebug() << "AutonomousSystem initialized with ID:" << m_id;
}

AutonomousSystem::~AutonomousSystem()
{
    m_topologyController.reset();
    m_topologyBuilder.reset();
}

int AutonomousSystem::getId() const
{
    return m_id;
}

void AutonomousSystem::connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS)
{
    if (!m_topologyController)
    {
        qWarning() << "TopologyController not initialized. Cannot connect to other ASes.";
        return;
    }
    m_topologyController->connectToOtherAS(allAS);
}

const std::vector<QSharedPointer<Router>> &AutonomousSystem::getRouters() const
{
    return m_topologyBuilder->getRouters();
}

const std::vector<QSharedPointer<PC>> &AutonomousSystem::getPCs() const
{
    return m_topologyBuilder->getPCs();
}

QSharedPointer<TopologyController> AutonomousSystem::getTopologyController() const
{
    return m_topologyController;
}
