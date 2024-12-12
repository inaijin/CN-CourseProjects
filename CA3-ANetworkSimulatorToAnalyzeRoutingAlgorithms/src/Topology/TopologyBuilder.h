#ifndef TOPOLOGYBUILDER_H
#define TOPOLOGYBUILDER_H

#include <QObject>
#include <QJsonObject>
#include <QSharedPointer>
#include <vector>
#include "../Network/Router.h"
#include "../Network/PC.h"
#include "../Globals/IdAssignment.h"

class TopologyBuilder : public QObject
{
    Q_OBJECT

public:
    explicit TopologyBuilder(const QJsonObject &config, const IdAssignment &idAssignment, QObject *parent = nullptr);
    ~TopologyBuilder();

    void buildTopology();
    const std::vector<QSharedPointer<Router>> &getRouters() const;
    const std::vector<QSharedPointer<PC>> &getPCs() const;
    const QJsonObject &getConfig() const;

private:
    QJsonObject m_config;
    QString m_topologyType;

    const IdAssignment &m_idAssignment;

    std::vector<QSharedPointer<Router>> m_routers;
    std::vector<QSharedPointer<PC>> m_pcs;

    void createRouters();
    void createPCs();
    void setupTopology();
    void validateConfig() const;
    void configureDHCPServers();
};

#endif // TOPOLOGYBUILDER_H
