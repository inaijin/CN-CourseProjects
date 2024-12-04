#include "AutonomousSystem.h"
#include "Router.h"
#include "PC.h"
#include "../PortBindingManager/PortBindingManager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

AutonomousSystem::AutonomousSystem(const QJsonObject &config, QObject *parent)
    : QObject(parent)
{
    m_id = config.value("id").toInt();
    m_topologyType = config.value("topology_type").toString();
    m_nodeCount = config.value("node_count").toInt();

    // Convert "as_gateways" to std::vector<int>
    QJsonArray asGatewaysArray = config.value("as_gateways").toArray();
    for (const QJsonValue &value : asGatewaysArray)
    {
        m_asGateways.push_back(value.toInt());
    }

    // Convert "user_gateways" to std::vector<int>
    QJsonArray userGatewaysArray = config.value("user_gateways").toArray();
    for (const QJsonValue &value : userGatewaysArray)
    {
        m_userGateways.push_back(value.toInt());
    }

    m_dhcpServerId = config.value("dhcp_server").toInt();

    // Convert "broken_routers" to std::vector<int>
    QJsonArray brokenRoutersArray = config.value("broken_routers").toArray();
    for (const QJsonValue &value : brokenRoutersArray)
    {
        m_brokenRouters.push_back(value.toInt());
    }

    m_gateways = config.value("gateways").toArray();
    m_connectToAs = config.value("connect_to_as").toArray();

    createRouters();
    createPCs();
    setupTopology();
    setupGateways();
}

AutonomousSystem::~AutonomousSystem()
{
    // Clean up if needed
}

void AutonomousSystem::createRouters()
{
    QString baseIP = QString("192.168.%1.").arg(m_id * 100); // e.g., 192.168.100.x for AS1
    for (int i = 1; i <= m_nodeCount; ++i)
    {
        if (std::find(m_brokenRouters.begin(), m_brokenRouters.end(), i) != m_brokenRouters.end())
        {
            continue; // Skip broken routers
        }

        QString routerIP = baseIP + QString::number(i);
        auto router = QSharedPointer<Router>::create(i, routerIP, this);
        m_routers.push_back(router);
    }
}

void AutonomousSystem::createPCs()
{
    for (const QJsonValue &gatewayValue : qAsConst(m_gateways))
    {
        QJsonObject gatewayObj = gatewayValue.toObject();
        int nodeId = gatewayObj.value("node").toInt();
        QJsonArray userArray = gatewayObj.value("users").toArray();

        for (const QJsonValue &userValue : qAsConst(userArray))
        {
            int userId = userValue.toInt();
            // Generate PC IP addresses
            QString pcIP = QString("192.168.%1.%2").arg(m_id * 100).arg(userId);
            auto pc = QSharedPointer<PC>::create(userId, pcIP, this);
            m_pcs.push_back(pc);

            // Find the corresponding router to connect
            auto routerIt = std::find_if(m_routers.begin(), m_routers.end(),
                                         [nodeId](const QSharedPointer<Router> &r) { return r->getId() == nodeId; });
            if (routerIt != m_routers.end())
            {
                // Bind PC to Router
                PortBindingManager bindingManager;
                bindingManager.bind((*routerIt)->getAvailablePort(), pc->getPort());
            }
        }
    }
}

void AutonomousSystem::connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS)
{
    // Implement the logic to connect to other Autonomous Systems based on m_connectToAs

    for (const QJsonValue &connectValue : qAsConst(m_connectToAs))
    {
        QJsonObject connectObj = connectValue.toObject();
        int targetASId = connectObj.value("id").toInt();
        QJsonArray gatewayPairs = connectObj.value("gateway_pairs").toArray();

        // Find the target AS
        auto targetASIt = std::find_if(allAS.begin(), allAS.end(),
                                                [targetASId](const QSharedPointer<AutonomousSystem> &as) { return as->getId() == targetASId; });

        if (targetASIt != allAS.end())
        {
            for (const QJsonValue &pairValue : qAsConst(gatewayPairs))
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
                    PortBindingManager bindingManager;
                    bindingManager.bind((*localRouterIt)->getAvailablePort(), (*remoteRouterIt)->getAvailablePort());
                }
            }
        }
    }
}

void AutonomousSystem::setupTopology()
{
    if (m_topologyType == "Mesh")
    {
        // Implement the mesh topology
        for (const auto &routerA : qAsConst(m_routers))
        {
            for (const auto &routerB : qAsConst(m_routers))
            {
                if (routerA != routerB)
                {
                    PortBindingManager bindingManager;
                    bindingManager.bind(routerA->getAvailablePort(), routerB->getAvailablePort());
                }
            }
        }
    }
    else if (m_topologyType == "RingStar")
    {
        // Implement the ring-star topology
        // Example: Connect routers in a ring
        for (size_t i = 0; i < m_routers.size(); ++i)
        {
            auto routerA = m_routers[i];
            auto routerB = m_routers[(i + 1) % m_routers.size()]; // Next router in the ring
            PortBindingManager bindingManager;
            bindingManager.bind(routerA->getAvailablePort(), routerB->getAvailablePort());
        }

        // Connect all routers to central hub (assuming first router is the hub)
        auto hubRouter = m_routers.front();
        for (size_t i = 1; i < m_routers.size(); ++i)
        {
            PortBindingManager bindingManager;
            bindingManager.bind(hubRouter->getAvailablePort(), m_routers[i]->getAvailablePort());
        }
    }
    else if (m_topologyType == "Torus")
    {
        // Implement torus topology if needed
    }
    else
    {
        qWarning() << "Unknown topology type:" << m_topologyType;
    }
}

void AutonomousSystem::setupGateways()
{
    // Implement any additional setup for gateways if needed
}
