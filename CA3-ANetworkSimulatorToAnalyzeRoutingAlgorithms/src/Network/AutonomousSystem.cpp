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

void AutonomousSystem::createRouters()
{
    // Assuming IP addresses are generated based on AS ID and router IDs
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
    // Iterate over gateways and create PCs
    for (const QJsonValue &gatewayValue : m_gateways)
    {
        QJsonObject gatewayObj = gatewayValue.toObject();
        int nodeId = gatewayObj.value("node").toInt();
        QJsonArray userArray = gatewayObj.value("users").toArray();

        for (const QJsonValue &userValue : userArray)
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
