#include "AutonomousSystem.h"
#include "Router.h"
#include "PC.h"
#include "PortBindingManager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

AutonomousSystem::AutonomousSystem(const QJsonObject &config, QObject *parent)
    : QObject(parent)
{
    m_id = config.value("id").toInt();
    m_topologyType = config.value("topology_type").toString();
    m_nodeCount = config.value("node_count").toInt();
    m_asGateways = config.value("as_gateways").toVariant().toList().toVector().toStdVector();
    m_userGateways = config.value("user_gateways").toVariant().toList().toVector().toStdVector();
    m_dhcpServerId = config.value("dhcp_server").toInt();
    m_brokenRouters = config.value("broken_routers").toVariant().toList().toVector().toStdVector();
    m_gateways = config.value("gateways").toArray();
    m_connectToAs = config.value("connect_to_as").toArray();

    createRouters();
    createPCs();
    setupTopology();
    setupGateways();
}

AutonomousSystem::~AutonomousSystem()
{
    // Cleanup if necessary
}

int AutonomousSystem::getId() const
{
    return m_id;
}

void AutonomousSystem::createRouters()
{
    for (int i = 1; i <= m_nodeCount; ++i)
    {
        if (std::find(m_brokenRouters.begin(), m_brokenRouters.end(), i) != m_brokenRouters.end())
        {
            continue; // Skip broken routers
        }

        auto router = QSharedPointer<Router>::create(i, this);
        m_routers.push_back(router);
    }
}

void AutonomousSystem::createPCs()
{
    // Iterate over gateways and create PCs
    for (const QJsonValue &gatewayValue : m_gateways)
    {
        QJsonObject gatewayObj = gatewayValue.toObject();
        int nodeId = gatewayObj.value("node").toInt();
        QJsonArray userArray = gatewayObj.value("users").toArray();

        for (const QJsonValue &userValue : userArray)
        {
            int userId = userValue.toInt();
            auto pc = QSharedPointer<PC>::create(userId, this);
            m_pcs.push_back(pc);

            // Find the corresponding router to connect
            auto routerIt = std::find_if(m_routers.begin(), m_routers.end(),
                                         [nodeId](const QSharedPointer<Router> &r) { return r->getId() == nodeId; });
            if (routerIt != m_routers.end())
            {
                // Bind PC to Router
                PortBindingManager::bind((*routerIt)->getAvailablePort(), pc->getPort());
            }
        }
    }
}

void AutonomousSystem::setupTopology()
{
    // Implement the topology based on m_topologyType (Mesh, RingStar, Torus)
    if (m_topologyType == "Mesh")
    {
        // Fully connect routers
        for (const auto &routerA : m_routers)
        {
            for (const auto &routerB : m_routers)
            {
                if (routerA != routerB)
                {
                    PortBindingManager::bind(routerA->getAvailablePort(), routerB->getAvailablePort());
                }
            }
        }
    }
    else if (m_topologyType == "RingStar")
    {
        // Implement RingStar topology
        // Simplified example: connect routers in a ring
        for (size_t i = 0; i < m_routers.size(); ++i)
        {
            auto routerA = m_routers[i];
            auto routerB = m_routers[(i + 1) % m_routers.size()]; // Next router in ring
            PortBindingManager::bind(routerA->getAvailablePort(), routerB->getAvailablePort());
        }

        // Connect all routers to central hub (assuming first router is the hub)
        auto hubRouter = m_routers.front();
        for (size_t i = 1; i < m_routers.size(); ++i)
        {
            PortBindingManager::bind(hubRouter->getAvailablePort(), m_routers[i]->getAvailablePort());
        }
    }
    else if (m_topologyType == "Torus")
    {
        // Implement Torus topology
        // For simplicity, not fully implemented here
    }
}

void AutonomousSystem::setupGateways()
{
    // Additional setup if necessary
}

void AutonomousSystem::connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS)
{
    // Implement connections to other AS based on m_connectToAs
    for (const QJsonValue &connectValue : m_connectToAs)
    {
        QJsonObject connectObj = connectValue.toObject();
        int targetASId = connectObj.value("id").toInt();
        QJsonArray gatewayPairs = connectObj.value("gateway_pairs").toArray();

        // Find the target AS
        auto targetASIt = std::find_if(allAS.begin(), allAS.end(),
                                                [targetASId](const QSharedPointer<AutonomousSystem> &as) { return as->getId() == targetASId; });

        if (targetASIt != allAS.end())
        {
            for (const QJsonValue &pairValue : gatewayPairs)
            {
                QJsonObject pairObj = pairValue.toObject();
                int gatewayId = pairObj.value("gateway").toInt();
                int connectToId = pairObj.value("connect_to").toInt();

                // Find routers in both AS
                auto localRouterIt = std::find_if(m_routers.begin(), m_routers.end(),
                                                          [gatewayId](const QSharedPointer<Router> &r) { return r->getId() == gatewayId; });
                auto remoteRouterIt = std::find_if((*targetASIt)->m_routers.begin(), (*targetASIt)->m_routers.end(),
                                                          [connectToId](const QSharedPointer<Router> &r) { return r->getId() == connectToId; });

                if (localRouterIt != m_routers.end() && remoteRouterIt != (*targetASIt)->m_routers.end())
                {
                    // Bind routers between AS
                    PortBindingManager::bind((*localRouterIt)->getAvailablePort(), (*remoteRouterIt)->getAvailablePort());
                }
            }
        }
    }
}
