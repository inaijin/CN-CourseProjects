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

int AutonomousSystem::getId() const
{
    return m_id;
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
        for (size_t i = 0; i < m_routers.size(); ++i)
        {
            auto routerA = m_routers[i];
            for (size_t j = i + 1; j < m_routers.size(); ++j)
            {
                auto routerB = m_routers[j];

                auto portA = routerA->getAvailablePort();
                auto portB = routerB->getAvailablePort();

                if (portA && portB)
                {
                    PortBindingManager bindingManager;
                    bindingManager.bind(portA, portB);
                }
                else
                {
                    qWarning() << "No available ports to bind between Router"
                               << routerA->getIPAddress() << "and Router" << routerB->getIPAddress();
                }
            }
        }
    }
    else if (m_topologyType == "RingStar")
    {
        if (m_routers.size() < 2)
        {
            qWarning() << "Not enough routers to form a ring-star topology.";
            return;
        }

        // Identify the hub router (assuming the first router)
        auto hubRouter = m_routers.front();

        // Define the ring routers (excluding the hub)
        QVector<QSharedPointer<Router>> ringRouters;

        // Only include routers that are intended to be part of the ring
        for (const auto &router : m_routers)
        {
            if (router != hubRouter)
            {
                QString ip = router->getIPAddress();
                if (ip == "192.168.200.2" || ip == "192.168.200.3" ||
                   ip == "192.168.200.4" || ip == "192.168.200.5")
                {
                    ringRouters.append(router);
                }
            }
        }

        // Form the ring connections
        for (int i = 0; i < ringRouters.size(); ++i)
        {
            auto routerA = ringRouters[i];
            auto routerB = ringRouters[(i + 1) % ringRouters.size()]; // Next router in the ring

            auto portA = routerA->getAvailablePort();
            auto portB = routerB->getAvailablePort();

            if (portA && portB)
            {
                PortBindingManager bindingManager;
                bindingManager.bind(portA, portB);
            }
            else
            {
                qWarning() << "No available ports to bind between Router"
                           << routerA->getIPAddress() << "and Router" << routerB->getIPAddress();
            }
        }

        // Connect each ring router to the hub router
        for (const auto &router : ringRouters)
        {
            auto portHub = hubRouter->getAvailablePort();
            auto portRouter = router->getAvailablePort();

            if (portHub && portRouter)
            {
                PortBindingManager bindingManager;
                bindingManager.bind(portHub, portRouter);
            }
            else
            {
                qWarning() << "No available ports to bind between Hub Router"
                           << hubRouter->getIPAddress() << "and Router" << router->getIPAddress();
            }
        }
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
