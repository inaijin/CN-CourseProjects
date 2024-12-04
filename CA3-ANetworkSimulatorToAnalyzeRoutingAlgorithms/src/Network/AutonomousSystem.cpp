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
