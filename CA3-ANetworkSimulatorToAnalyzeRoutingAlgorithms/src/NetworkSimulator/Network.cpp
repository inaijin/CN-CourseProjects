#include <QDebug>
#include <QJsonArray>
#include <unordered_map>

#include "Network.h"
#include "Topology/TopologyController.h"

Network::Network(const QJsonObject &config, QObject *parent)
    : QObject(parent), m_config(config)
{}

Network::~Network()
{}

void Network::initialize(const IdAssignment &idAssignment, const bool torus)
{
    createAutonomousSystems(idAssignment, torus);
    connectAutonomousSystems();
}

void Network::createAutonomousSystems(const IdAssignment &idAssignment, const bool torus)
{
    QJsonArray asArray = m_config.value("Autonomous_systems").toArray();
    for (const QJsonValue &asValue : asArray)
    {
        QJsonObject asObject = asValue.toObject();
        auto asInstance = QSharedPointer<AutonomousSystem>::create(asObject, idAssignment, torus);
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

std::vector<QSharedPointer<Router>> Network::getAllRouters() const {
    std::vector<QSharedPointer<Router>> allRouters;
    for (const auto &asInstance : m_autonomousSystems) {
        auto routers = asInstance->getRouters();
        allRouters.insert(allRouters.end(), routers.begin(), routers.end());
    }
    return allRouters;
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
            if (!router->isBroken())
                router->enableRIP();
        }
    }
    qDebug() << "RIP enabled on all routers.";
}

void Network::enableOSPFOnAllRouters()
{
    for (auto &asInstance : m_autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        for (auto &router : routers) {
            if (!router->isBroken())
                router->enableOSPF();
        }
    }
    qDebug() << "OSPF enabled on all routers.";
}

void Network::startBGP(RoutingProtocol protocolAS1, RoutingProtocol protocolAS2) {
    std::unordered_map<int, RoutingProtocol> asProtocolMap = {
     {1, protocolAS1},
     {2, protocolAS2}
    };

    for (auto &asInstance : m_autonomousSystems) {
        int asNum = asInstance->getId();

        auto protocolIt = asProtocolMap.find(asNum);
        if (protocolIt == asProtocolMap.end()) {
            qWarning() << "No routing protocol configured for AS" << asNum << ". Skipping.";
            continue;
        }

        RoutingProtocol currentProtocol = protocolIt->second;
        const auto &routers = asInstance->getRouters();

        for (auto &router : routers) {
            if (!router->isBroken()) {
                router->setASNum(asNum);

                switch (currentProtocol) {
                    case RoutingProtocol::RIP:
                        router->enableRIP();
                        break;
                    case RoutingProtocol::OSPF:
                        router->enableOSPF();
                        break;
                    default:
                        qWarning() << "Unsupported Routing Protocol for AS" << asNum;
                        break;
                }
            }
        }
    }
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

void Network::setupDirectRoutesForRouters(RoutingProtocol protocol)
{
    for (auto &asInstance : m_autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        for (auto &router : routers) {
            if (!router->isBroken()) {
                QString routerIP = router->getIPAddress();
                if (routerIP.isEmpty()) {
                    qWarning() << "Router" << router->getId() << "has no IP assigned yet, skipping direct route setup.";
                    continue;
                }

                QString mask = "255.255.255.255";
                int metric = 0;

                router->addRoute(routerIP, mask, "", metric, protocol);
                qDebug() << "Network: Added host route for Router" << router->getId() << ":" << routerIP << "/" << mask << "metric" << metric;
            }
        }
    }
}

void Network::finalizeRoutesAfterDHCP(RoutingProtocol protocol, bool bgp, RoutingProtocol protocolAS1, RoutingProtocol protocolAS2) {
    for (auto &asInstance : m_autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        int asNum = asInstance->getId();
        for (auto &router : routers) {
            if (!router->isBroken()) {
                if (!bgp) {
                    router->setupDirectNeighborRoutes(protocol, asInstance->getId(), bgp);
                } else {
                    if (asNum == 1) {
                        router->setupDirectNeighborRoutes(protocolAS1, asInstance->getId(), bgp);
                    } else {
                        router->setupDirectNeighborRoutes(protocolAS2, asInstance->getId(), bgp);
                    }
                }
            }
        }
    }
    qDebug() << "All routers have set direct neighbor routes.";
}

std::vector<QSharedPointer<AutonomousSystem>> Network::getAutonomousSystems() const {
    return m_autonomousSystems;
}

void Network::startEBGP() {
    for (auto &asInstance : m_autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        for (auto &router : routers) {
            if(router->isRouterBorder())
                router->startEBGP();
        }
    }
}

void Network::startIBGP() {
    for (auto &asInstance : m_autonomousSystems) {
        const auto &routers = asInstance->getRouters();
        for (auto &router : routers) {
            if(router->isRouterBorder())
                router->startIBGP();
        }
    }
}
