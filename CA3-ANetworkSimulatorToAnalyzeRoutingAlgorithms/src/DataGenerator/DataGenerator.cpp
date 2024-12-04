#include "DataGenerator.h"
#include <QDebug>

DataGenerator::DataGenerator(QObject *parent) :
    QObject(parent),
    m_lambda(1.0),
    m_distribution(m_lambda)
{
    // Initialize random engine with a seed for reproducibility
    std::random_device rd;
    m_generator.seed(rd());
}

void DataGenerator::setLambda(double lambda)
{
    m_lambda = lambda;
    m_distribution = std::poisson_distribution<int>(m_lambda);
}

void DataGenerator::setDestinations(const std::vector<QString> &destinations)
{
    m_destinations = destinations;
}

void DataGenerator::generatePackets()
{
    int numPackets = m_distribution(m_generator);
    // std::vector<QSharedPointer<Packet>> packets;

    for (int i = 0; i < numPackets; ++i)
    {
        // Randomly select a destination (ensuring it's not the source)
        if (m_destinations.empty())
        {
            qDebug() << "No destinations available for packet generation.";
            return;
        }

        // QString destination = m_destinations[std::rand() % m_destinations.size()];

        // Create a new packet
        // auto packet = QSharedPointer<Packet>::create();
        // packet->setDestination(destination);

        // packets.push_back(packet);
    }

    // emit packetsGenerated(packets);

    qDebug() << numPackets << "packets generated.";
}
