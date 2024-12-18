#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QSharedPointer>
#include <QJsonObject>
#include <vector>
#include "../Network/AutonomousSystem.h"
#include "../Globals/IdAssignment.h"

class Network : public QObject
{
    Q_OBJECT

public:
    explicit Network(const QJsonObject &config, QObject *parent = nullptr);
    ~Network();

    void initialize(const IdAssignment &idAssignment);
    void connectAutonomousSystems();
    void initiateDHCPPhase();
    void initiateDHCPPhaseForPC();
    void checkAssignedIP();
    void checkAssignedIPPC();

private:
    QJsonObject m_config;
    std::vector<QSharedPointer<AutonomousSystem>> m_autonomousSystems;

    void createAutonomousSystems(const IdAssignment &idAssignment);
};

#endif // NETWORK_H
