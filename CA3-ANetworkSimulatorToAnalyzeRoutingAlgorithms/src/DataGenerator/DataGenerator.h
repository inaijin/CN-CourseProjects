#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <random>
#include <vector>
#include <QString>
#include <QObject>
#include <QSharedPointer>

#include "../Network/PC.h"
#include "../Packet/Packet.h"

class DataGenerator : public QObject
{
    Q_OBJECT

public:
    explicit DataGenerator(QObject *parent = nullptr);
    ~DataGenerator() override = default;

    void setLambda(double lambda);
    void setSenders(const std::vector<QSharedPointer<PC>> &senders);
    void generatePackets();
    void loadConfig(const QString &configFilePath);

    std::vector<QSharedPointer<PC>> getSenders() const;
    std::vector<int> generatePoissonLoads(int numSamples, int timeScale);

Q_SIGNALS:
    void packetsGenerated(const std::vector<QSharedPointer<Packet>> &packets);

private:
    double m_lambda;
    int m_packetsPerSimulation = 150;

    std::default_random_engine m_generator;
    std::poisson_distribution<int> m_distribution;
    std::vector<QSharedPointer<PC>> m_senders;
};

#endif // DATAGENERATOR_H
