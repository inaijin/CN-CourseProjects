#include "TopologyBuilder.h"
#include "../Network/Router.h"
#include "../Network/PC.h"
#include "../PortBindingManager/PortBindingManager.h"
#include <QDebug>
#include <stdexcept>

TopologyBuilder::TopologyBuilder(const QJsonObject &config, QObject *parent)
    : QObject(parent), m_config(config)
{
    validateConfig();
    m_topologyType = config.value("topology_type").toString();
}

TopologyBuilder::~TopologyBuilder() {}

void TopologyBuilder::buildTopology()
{
    createRouters();
    createPCs();
    setupTopology();
}

void TopologyBuilder::validateConfig() const
{
    if (!m_config.contains("id") || !m_config.contains("node_count"))
        throw std::invalid_argument("Invalid configuration: Missing required keys 'id' or 'node_count'.");
}

void TopologyBuilder::createRouters()
{
    QString baseIP = QString("192.168.%1.").arg(m_config.value("id").toInt() * 100);
    int nodeCount = m_config.value("node_count").toInt();
    int portCount = m_config.value("router_port_count").toInt(6);

    QJsonArray brokenRoutersArray = m_config.value("broken_routers").toArray();
    std::vector<int> brokenRouters;
    for (const QJsonValue &value : brokenRoutersArray)
        brokenRouters.push_back(value.toInt());

    for (int i = 1; i <= nodeCount; ++i)
    {
        if (std::find(brokenRouters.begin(), brokenRouters.end(), i) != brokenRouters.end())
        {
            qWarning() << "Skipping broken router with ID:" << i;
            continue;
        }

        QString routerIP = baseIP + QString::number(i);
        auto router = QSharedPointer<Router>::create(i, routerIP, portCount, this);
        m_routers.push_back(router);
    }
}

void TopologyBuilder::createPCs()
{
    QJsonArray gateways = m_config.value("gateways").toArray();
    for (const QJsonValue &gatewayValue : gateways)
    {
        QJsonObject gatewayObj = gatewayValue.toObject();
        int nodeId = gatewayObj.value("node").toInt();
        QJsonArray userArray = gatewayObj.value("users").toArray();

        auto routerIt = std::find_if(m_routers.begin(), m_routers.end(),
                                              [nodeId](const QSharedPointer<Router> &r) { return r->getId() == nodeId; });

        if (routerIt == m_routers.end())
        {
            qWarning() << "Gateway router with ID:" << nodeId << "not found.";
            continue;
        }

        for (const QJsonValue &userValue : userArray)
        {
            int userId = userValue.toInt();
            QString pcIP = QString("192.168.%1.%2").arg(m_config.value("id").toInt() * 100).arg(userId);
            auto pc = QSharedPointer<PC>::create(userId, pcIP, this);
            m_pcs.push_back(pc);

            PortBindingManager bindingManager;
            bindingManager.bind((*routerIt)->getAvailablePort(), pc->getPort());
        }
    }
}

void TopologyBuilder::setupTopology()
{
    if (m_topologyType == "Mesh")
    {
        for (size_t i = 0; i < m_routers.size(); ++i)
        {
            auto routerA = m_routers[i];
            for (size_t j = i + 1; j < m_routers.size(); ++j)
            {
                auto routerB = m_routers[j];
                PortBindingManager bindingManager;
                bindingManager.bind(routerA->getAvailablePort(), routerB->getAvailablePort());
            }
        }
    }
    else if (m_topologyType == "RingStar")
    {
        auto hubRouter = m_routers.front();
        QVector<QSharedPointer<Router>> ringRouters;

        for (const auto &router : m_routers)
        {
            if (router != hubRouter)
                ringRouters.append(router);
        }

        for (int i = 0; i < ringRouters.size(); ++i)
        {
            auto routerA = ringRouters[i];
            auto routerB = ringRouters[(i + 1) % ringRouters.size()];
            PortBindingManager bindingManager;
            bindingManager.bind(routerA->getAvailablePort(), routerB->getAvailablePort());
        }

        for (const auto &router : ringRouters)
        {
            PortBindingManager bindingManager;
            bindingManager.bind(hubRouter->getAvailablePort(), router->getAvailablePort());
        }
    }
}

const std::vector<QSharedPointer<Router>> &TopologyBuilder::getRouters() const
{
    return m_routers;
}

const std::vector<QSharedPointer<PC>> &TopologyBuilder::getPCs() const
{
    return m_pcs;
}

const QJsonObject &TopologyBuilder::getConfig() const
{
    return m_config;
}
