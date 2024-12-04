#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QSharedPointer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include "../Network/AutonomousSystem.h"
#include <vector>

class Simulator : public QObject
{
    Q_OBJECT

public:
    explicit Simulator(QObject *parent = nullptr);
    ~Simulator();

    bool loadConfig(const QString &configFilePath);
    void initializeNetwork();
    void startSimulation();

private:
    QJsonObject m_config;
    std::vector<QSharedPointer<AutonomousSystem>> m_autonomousSystems;

    void createAutonomousSystems();
    void connectAutonomousSystems();
};

#endif // SIMULATOR_H
