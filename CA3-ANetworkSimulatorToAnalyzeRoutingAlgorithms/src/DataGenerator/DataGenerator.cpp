#include "DataGenerator.h"
#include <QDebug>

DataGenerator::DataGenerator(QObject *parent) :
    QObject(parent),
    m_lambda(1.0),
    m_distribution(m_lambda)
{
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
    std::vector<QSharedPointer<Packet>> packets;

    for (int i = 0; i < numPackets; ++i)
    {
        if (m_destinations.empty())
        {
            qDebug() << "No destinations available for packet generation.";
            return;
        }

        QString destination = m_destinations[std::rand() % m_destinations.size()];

        auto packet = QSharedPointer<Packet>::create(PacketType::Data, "GeneratedPayload");
        packet->addToPath(destination);

        packets.push_back(packet);
    }

    emit packetsGenerated(packets);

    qDebug() << numPackets << "packets generated and emitted.";
}

// yeki besazi ba poision bad inkaro bokonim
// be khodesh nafrestim
