#ifndef ROUTERREGISTRY_H
#define ROUTERREGISTRY_H

#include <vector>
#include <QSharedPointer>
#include <../Network/Router.h>

class RouterRegistry {
public:
    static std::vector<QSharedPointer<Router>> allRouters;

    static void addRouters(const std::vector<QSharedPointer<Router>> &routers) {
        for (auto &r : routers) {
            allRouters.push_back(r);
        }
    }

    static QSharedPointer<Router> findRouterById(int routerId) {
        for (auto &r : allRouters) {
            if (r->getId() == routerId) {
                return r;
            }
        }
        return nullptr;
    }
};

#endif // ROUTERREGISTRY_H
