#include "TopologyController.h"
#include "TopologyBuilder.h"
#include "../Network/Router.h"
#include "../Network/AutonomousSystem.h"
#include "../PortBindingManager/PortBindingManager.h"
#include <QDebug>

TopologyController::TopologyController(const QSharedPointer<TopologyBuilder> &builder, QObject *parent)
    : QObject(parent), m_builder(builder)
{
    // MEOW
}

TopologyController::~TopologyController() {}

void TopologyController::validateTopology()
{
    qDebug() << "Validating topology...";
    const auto &routers = m_builder->getRouters();
    for (const auto &router : routers)
    {
        if (!router->getAvailablePort())
            qWarning() << "Router" << router->getId() << "has no available ports.";
    }
}

void TopologyController::connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS)
{
    const auto &routers = m_builder->getRouters();
    QJsonArray connectToAsArray = m_builder->getConfig().value("connect_to_as").toArray();

    for (const QJsonValue &connectValue : connectToAsArray)
    {
        QJsonObject connectObj = connectValue.toObject();
        int targetASId = connectObj.value("id").toInt();
        QJsonArray gatewayPairs = connectObj.value("gateway_pairs").toArray();

        auto targetASIt = std::find_if(allAS.begin(), allAS.end(),
                                                [targetASId](const QSharedPointer<AutonomousSystem> &as) { return as->getId() == targetASId; });

        if (targetASIt != allAS.end())
        {
            const auto &targetRouters = (*targetASIt)->getRouters();
            for (const QJsonValue &pairValue : gatewayPairs)
            {
                QJsonObject pairObj = pairValue.toObject();
                int gatewayId = pairObj.value("gateway").toInt();
                int connectToId = pairObj.value("connect_to").toInt();

                auto localRouterIt = std::find_if(routers.begin(), routers.end(),
                                                          [gatewayId](const QSharedPointer<Router> &r) { return r->getId() == gatewayId; });
                auto remoteRouterIt = std::find_if(targetRouters.begin(), targetRouters.end(),
                                                          [connectToId](const QSharedPointer<Router> &r) { return r->getId() == connectToId; });

                if (localRouterIt != routers.end() && remoteRouterIt != targetRouters.end())
                {
                    PortBindingManager bindingManager;
                    bindingManager.bind((*localRouterIt)->getAvailablePort(), (*remoteRouterIt)->getAvailablePort());
                }
            }
        }
    }
}
