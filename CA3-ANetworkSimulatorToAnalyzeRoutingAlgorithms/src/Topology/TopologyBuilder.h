#ifndef TOPOLOGYBUILDER_H
#define TOPOLOGYBUILDER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QSharedPointer>
#include <vector>

class Router;
class PC;

class TopologyBuilder : public QObject
{
    Q_OBJECT

public:
    explicit TopologyBuilder(const QJsonObject &config, QObject *parent = nullptr);
    ~TopologyBuilder();

    void buildTopology();
    const std::vector<QSharedPointer<Router>> &getRouters() const;
    const std::vector<QSharedPointer<PC>> &getPCs() const;
    const QJsonObject &getConfig() const;

private:
    QJsonObject m_config;
    QString m_topologyType;

    std::vector<QSharedPointer<Router>> m_routers;
    std::vector<QSharedPointer<PC>> m_pcs;

    void createRouters();
    void createPCs();
    void setupTopology();
};

#endif // TOPOLOGYBUILDER_H
