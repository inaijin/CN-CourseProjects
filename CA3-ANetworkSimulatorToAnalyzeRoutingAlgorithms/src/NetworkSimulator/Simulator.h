#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QSharedPointer>
#include <QJsonObject>
#include <QString>
#include "Network.h"

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
    QSharedPointer<Network> m_network;
};

#endif // SIMULATOR_H
