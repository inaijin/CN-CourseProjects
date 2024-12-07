#include "TopologyController.h"
#include "TopologyBuilder.h"
#include "Router.h"
#include "PC.h"
#include "../PortBindingManager/PortBindingManager.h"
#include <QDebug>

TopologyController::TopologyController(const QSharedPointer<TopologyBuilder> &builder, QObject *parent)
    : QObject(parent), m_builder(builder)
{
    validateTopology();
}

TopologyController::~TopologyController() {}

void TopologyController::validateTopology() const
{
    qDebug() << "Validating topology...";
}

void TopologyController::connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS)
{
    const auto &routers = m_builder->getRouters(); // Get routers from TopologyBuilder
    QJsonArray connectToAsArray = m_builder->getConfig().value("connect_to_as").toArray(); // Configuration for connections

    for (const QJsonValue &connectValue : qAsConst(connectToAsArray))
    {
        QJsonObject connectObj = connectValue.toObject();
        int targetASId = connectObj.value("id").toInt();
        QJsonArray gatewayPairs = connectObj.value("gateway_pairs").toArray();

        auto targetASIt = std::find_if(allAS.begin(), allAS.end(),
                                       [targetASId](const QSharedPointer<AutonomousSystem> &as) { return as->getId() == targetASId; });

        if (targetASIt != allAS.end())
        {
            const auto &targetRouters = (*targetASIt)->getRouters(); // Access target AS routers
            for (const QJsonValue &pairValue : qAsConst(gatewayPairs))
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
