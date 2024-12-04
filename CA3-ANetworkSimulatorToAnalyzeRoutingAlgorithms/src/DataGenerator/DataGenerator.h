#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QSharedPointer>
#include <random>
#include <vector>
#include <QString>

// #include "Packet.h"

class DataGenerator : public QObject
{
    Q_OBJECT

public:
    explicit DataGenerator(QObject *parent = nullptr);
    ~DataGenerator() override = default;

    void setLambda(double lambda);
    void setDestinations(const std::vector<QString> &destinations);
    void generatePackets();

Q_SIGNALS:
    // void packetsGenerated(const std::vector<QSharedPointer<Packet>> &packets);

private:
    double m_lambda; // Poisson distribution parameter
    std::default_random_engine m_generator;
    std::poisson_distribution<int> m_distribution;
    std::vector<QString> m_destinations; // Available destinations
};

#endif // DATAGENERATOR_H
