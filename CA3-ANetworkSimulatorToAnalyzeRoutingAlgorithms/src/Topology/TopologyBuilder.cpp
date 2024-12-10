#include "TopologyBuilder.h"
#include "../PortBindingManager/PortBindingManager.h"
#include <QDebug>
#include <stdexcept>
#include <QJsonArray>

TopologyBuilder::TopologyBuilder(const QJsonObject &config, const IdAssignment &idAssignment, QObject *parent)
    : QObject(parent), m_config(config), m_idAssignment(idAssignment)
{
    validateConfig();
    m_topologyType = config.value("topology_type").toString();
}

TopologyBuilder::~TopologyBuilder() {}

void TopologyBuilder::buildTopology()
{
    createRouters();
    setupTopology();
    createPCs();
}

void TopologyBuilder::validateConfig() const
{
    if (!m_config.contains("id") || !m_config.contains("node_count"))
        throw std::invalid_argument("Invalid configuration: Missing required keys 'id' or 'node_count'.");
}

void TopologyBuilder::createRouters()
{
    int asId = m_config.value("id").toInt();
    AsIdRange range;
    if (!m_idAssignment.getAsIdRange(asId, range)) {
        throw std::runtime_error("ID range not found for AS");
    }

    int nodeCount = m_config.value("node_count").toInt();
    QString baseIP = QString("192.168.%1.").arg(asId * 100);
    int portCount = m_config.value("router_port_count").toInt(6);

    if ((range.routerEndId - range.routerStartId + 1) != nodeCount)
        throw std::runtime_error("Router count doesn't match assigned range.");

    QJsonArray brokenRoutersArray = m_config.value("broken_routers").toArray();
    std::vector<int> brokenRouters;
    for (const QJsonValue &value : brokenRoutersArray)
        brokenRouters.push_back(value.toInt());

    for (int i = 0; i < nodeCount; ++i)
    {
        int routerId = range.routerStartId + i;
        if (std::find(brokenRouters.begin(), brokenRouters.end(), routerId) != brokenRouters.end())
        {
            qWarning() << "Skipping broken router with ID:" << routerId;
            continue;
        }

        QString routerIP = baseIP + QString::number(routerId);
        auto router = QSharedPointer<Router>::create(routerId, routerIP, portCount, this);
        m_routers.push_back(router);
    }
}

void TopologyBuilder::createPCs()
{
    int asId = m_config.value("id").toInt();
    AsIdRange range;
    if (!m_idAssignment.getAsIdRange(asId, range)) {
        qWarning() << "ID range not found for AS" << asId;
        return;
    }

    QJsonArray gateways = m_config.value("gateways").toArray();
    if (gateways.isEmpty())
    {
        qWarning() << "No gateways defined in the configuration.";
        return;
    }

    QString baseIP = QString("192.168.%1.").arg(asId * 100);

    for (const QJsonValue &gatewayValue : gateways)
    {
        QJsonObject gatewayObj = gatewayValue.toObject();
        if (!gatewayObj.contains("node") || !gatewayObj.contains("users"))
        {
            qWarning() << "Invalid gateway definition.";
            continue;
        }

        int gatewayNodeId = gatewayObj.value("node").toInt();
        QJsonArray userArray = gatewayObj.value("users").toArray();

        auto routerIt = std::find_if(m_routers.begin(), m_routers.end(),
                                                [gatewayNodeId](const QSharedPointer<Router> &r) { return r->getId() == gatewayNodeId; });

        if (routerIt == m_routers.end())
        {
            qWarning() << "Gateway router with ID:" << gatewayNodeId << "not found.";
            continue;
        }

        for (const QJsonValue &userValue : userArray)
        {
            int pcId = userValue.toInt();
            if (pcId <= 0)
            {
                qWarning() << "Invalid PC ID:" << pcId;
                continue;
            }

            QString pcIP = baseIP + QString::number(pcId);
            auto pc = QSharedPointer<PC>::create(pcId, pcIP, this);
            m_pcs.push_back(pc);

            PortBindingManager bindingManager;
            bindingManager.bind((*routerIt)->getAvailablePort(), pc->getPort());

            qDebug() << "PC" << pcId << "bound to Router" << gatewayNodeId;
        }
    }
}

void TopologyBuilder::setupTopology()
{
    if (m_topologyType == "Mesh")
    {
        int rows = 4;
        int columns = 4;

        QSet<QPair<int, int>> connectedPairs;

        for (int i = 0; i < static_cast<int>(m_routers.size()); ++i)
        {
            int routerId = m_routers[i]->getId();
            int row = i / columns;
            int col = i % columns;

            QVector<int> neighbors;
            if (row > 0)
                neighbors.append(m_routers[(row - 1) * columns + col]->getId());
            if (row < rows - 1)
                neighbors.append(m_routers[(row + 1) * columns + col]->getId());
            if (col > 0)
                neighbors.append(m_routers[row * columns + (col - 1)]->getId());
            if (col < columns - 1)
                neighbors.append(m_routers[row * columns + (col + 1)]->getId());

            for (int neighborId : neighbors)
            {
                QPair<int, int> pair = qMakePair(qMin(routerId, neighborId), qMax(routerId, neighborId));
                if (connectedPairs.contains(pair))
                    continue;

                auto neighborIt = std::find_if(m_routers.begin(), m_routers.end(),
                                               [neighborId](const QSharedPointer<Router> &r) { return r->getId() == neighborId; });

                if (neighborIt != m_routers.end())
                {
                    PortBindingManager bindingManager;
                    bindingManager.bind(m_routers[i]->getAvailablePort(), (*neighborIt)->getAvailablePort());
                    connectedPairs.insert(pair);
                }
            }
        }
    }
    else if (m_topologyType == "RingStar")
    {
        auto hubRouter = *std::max_element(m_routers.begin(), m_routers.end(),
                                                                      [](const QSharedPointer<Router> &a, const QSharedPointer<Router> &b) {
                                               return a->getId() < b->getId();
                                           });

        QVector<QSharedPointer<Router>> ringRouters;

        for (const auto &router : m_routers)
        {
            if (router != hubRouter)
                ringRouters.append(router);
        }

        std::sort(ringRouters.begin(), ringRouters.end(),
                  [](const QSharedPointer<Router> &a, const QSharedPointer<Router> &b) {
                      return a->getId() < b->getId();
                  });

        for (int i = 0; i < ringRouters.size(); ++i)
        {
            auto routerA = ringRouters[i];
            auto routerB = ringRouters[(i + 1) % ringRouters.size()];
            PortBindingManager bindingManager;
            bindingManager.bind(routerA->getAvailablePort(), routerB->getAvailablePort());
        }

        for (int i = 0; i < ringRouters.size(); i += 2)
        {
            auto router = ringRouters[i];
            PortBindingManager bindingManager;
            bindingManager.bind(router->getAvailablePort(), hubRouter->getAvailablePort());
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
