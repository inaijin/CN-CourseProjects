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

void Network::initiateDHCPPhaseForPC()
{
    for (const auto &asInstance : m_autonomousSystems)
    {
        auto topologyController = asInstance->getTopologyController();
        if (topologyController)
        {
            topologyController->initiateDHCPPhaseForPC();
        }
        else
        {
            qWarning() << "TopologyController is not initialized for AS.";
        }
    }
}

void Network::checkAssignedIP() {
    for (const auto &asInstance : m_autonomousSystems)
    {
        auto topologyController = asInstance->getTopologyController();
        if (topologyController)
        {
            topologyController->checkAssignedIP();
        }
        else
        {
            qWarning() << "TopologyController is not initialized for AS.";
        }
    }
}

void Network::checkAssignedIPPC() {
    for (const auto &asInstance : m_autonomousSystems)
    {
        auto topologyController = asInstance->getTopologyController();
        if (topologyController)
        {
            topologyController->checkAssignedIPPC();
        }
        else
        {
            qWarning() << "TopologyController is not initialized for AS.";
        }
    }
}

void Network::enableRIPOnAllRouters()
{
    for (auto &asInstance : m_autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        for (auto &router : routers) {
            router->enableRIP();
        }
    }
    qDebug() << "RIP enabled on all routers.";
}

void Network::printAllRoutingTables()
{
    qDebug() << "Printing all routing tables:";
    for (auto &asInstance : m_autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        for (auto &router : routers) {
            router->printRoutingTable();
        }
    }
}

void Network::setupDirectRoutesForRouters()
{
    for (auto &asInstance : m_autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        for (auto &router : routers) {
            QString routerIP = router->getIPAddress();
            if (routerIP.isEmpty()) {
                qWarning() << "Router" << router->getId() << "has no IP assigned yet, skipping direct route setup.";
                continue;
            }

            QString mask = "255.255.255.255";
            int metric = 0;

            router->addRoute(routerIP, mask, "", metric, RoutingProtocol::RIP);
            qDebug() << "Network: Added host route for Router" << router->getId() << ":" << routerIP << "/" << mask << "metric" << metric;
        }
    }
}

void Network::finalizeRoutesAfterDHCP() {
    for (auto &asInstance : m_autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        for (auto &router : routers) {
            qDebug() << "Router " << "begaii " << router->getId();
            router->setupDirectNeighborRoutes();
        }
    }
    qDebug() << "All routers have set direct neighbor routes.";
}
