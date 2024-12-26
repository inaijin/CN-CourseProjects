#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QSharedPointer>
#include <QJsonObject>
#include <vector>
#include "../Network/AutonomousSystem.h"
#include "../Globals/IdAssignment.h"
#include "../Network/Router.h"

class Network : public QObject
{
    Q_OBJECT

public:
    explicit Network(const QJsonObject &config, QObject *parent = nullptr);
    ~Network();

    void initialize(const IdAssignment &idAssignment, const bool torus);
    void connectAutonomousSystems();
    void initiateDHCPPhase();
    void initiateDHCPPhaseForPC();
    void checkAssignedIP();
    void checkAssignedIPPC();

    void enableRIPOnAllRouters();
    void enableOSPFOnAllRouters();
    void printAllRoutingTables();
    void setupDirectRoutesForRouters(RoutingProtocol protocol);

    void finalizeRoutesAfterDHCP(RoutingProtocol protocol);
    std::vector<QSharedPointer<Router>> getAllRouters() const;
    std::vector<QSharedPointer<AutonomousSystem>> getAutonomousSystems() const;

private:
    QJsonObject m_config;
    std::vector<QSharedPointer<AutonomousSystem>> m_autonomousSystems;

    void createAutonomousSystems(const IdAssignment &idAssignment, bool torus);
};

#endif // NETWORK_H
