#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QSharedPointer>

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
    QSharedPointer<Network> getNetwork() { return m_network; }

    bool configureFromCommandLine(const QStringList& arguments);
    void printTopologyVisualization();
    void printAsciiDiagram(bool addTorus);

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
    std::chrono::milliseconds m_cycleDuration;

    std::chrono::milliseconds parseDuration(const QString &durationStr);

    void preAssignIDs();

    // Configuration variables
    bool useBGP;
    int firstASAlgo;
    int secondASAlgo;
    int mainAlgo;
    bool addTorus;

    void setUseBGP(bool bgp);
    void setFirstASAlgo(int algo);
    void setSecondASAlgo(int algo);
    void setMainAlgo(int algo);
    void setAddTorus(bool torus);

    bool promptYesNo(const QString& prompt);
    int promptForAlgorithm(const QString& prompt);
};

#endif // SIMULATOR_H
