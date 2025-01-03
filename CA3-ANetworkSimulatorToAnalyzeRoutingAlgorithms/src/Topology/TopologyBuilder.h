#ifndef TOPOLOGYBUILDER_H
#define TOPOLOGYBUILDER_H

#include <vector>
#include <QObject>
#include <QJsonObject>
#include <QSharedPointer>

#include "../Network/PC.h"
#include "../Network/Router.h"
#include "../Globals/IdAssignment.h"

class TopologyBuilder : public QObject
{
    Q_OBJECT

public:
    explicit TopologyBuilder(const QJsonObject &config, const IdAssignment &idAssignment, QObject *parent = nullptr);
    ~TopologyBuilder();

    void buildTopology(bool torus);
    const std::vector<QSharedPointer<Router>> &getRouters() const;
    const std::vector<QSharedPointer<PC>> &getPCs() const;
    const QJsonObject &getConfig() const;
    int getASIdForRouter(int routerId) const;
    void makeMeshTorus();
    QSharedPointer<Router> findRouterById(int routerId) const;

private:
    QJsonObject m_config;
    QString m_topologyType;
    QSet<QPair<int, int>> m_connectedPairs;

    const IdAssignment &m_idAssignment;

    std::vector<QSharedPointer<Router>> m_routers;
    std::vector<QSharedPointer<PC>> m_pcs;
    std::map<int, int> m_routerToASMap;

    void createRouters();
    void createPCs();
    void setupTopology();
    void validateConfig() const;
    void configureDHCPServers();
};

#endif // TOPOLOGYBUILDER_H
