#ifndef AUTONOMOUSSYSTEM_H
#define AUTONOMOUSSYSTEM_H

#include <QObject>
#include <QSharedPointer>
#include <QJsonObject>
#include <QJsonArray>
#include <vector>

class Router;
class PC;

class AutonomousSystem : public QObject
{
    Q_OBJECT

public:
    explicit AutonomousSystem(const QJsonObject &config, QObject *parent = nullptr);
    ~AutonomousSystem();

    int getId() const;
    void connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS);

private:
    int m_id;
    QString m_topologyType;
    int m_nodeCount;
    int m_portCount;
    std::vector<int> m_asGateways;
    std::vector<int> m_userGateways;
    int m_dhcpServerId;
    std::vector<int> m_brokenRouters;
    QJsonArray m_gateways;
    QJsonArray m_connectToAs;

    std::vector<QSharedPointer<Router>> m_routers;
    std::vector<QSharedPointer<PC>> m_pcs;

    void createRouters();
    void createPCs();
    void setupTopology();
    void setupGateways();
};

#endif // AUTONOMOUSSYSTEM_H
