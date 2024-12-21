#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QSharedPointer>
#include <QJsonObject>
#include <QString>
#include "Network.h"
#include "IdAssignment.h"
#include "DataGenerator/DataGenerator.h"
#include "../MetricsCollector/MetricsCollector.h"

class Simulator : public QObject
{
    Q_OBJECT

public:
    explicit Simulator(QObject *parent = nullptr);
    ~Simulator();

    bool loadConfig(const QString &configFilePath);
    void initializeNetwork();
    void startSimulation();
    void initiateDHCPPhase();
    void checkAssignedIP();
    void checkAssignedIPPC();
    void initiatePacketSending();

public slots:
    void onConvergenceDetected();
    void handleGeneratedPackets(const std::vector<QSharedPointer<Packet>> &packets);

signals:
    void convergenceReached();

private:
    QJsonObject m_config;
    QSharedPointer<Network> m_network;
    QSharedPointer<DataGenerator> m_dataGenerator;
    QSharedPointer<MetricsCollector> m_metricsCollector;
    IdAssignment m_idAssignment;

    void preAssignIDs();
};

#endif // SIMULATOR_H
