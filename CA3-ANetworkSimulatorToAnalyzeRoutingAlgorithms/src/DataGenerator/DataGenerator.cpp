#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include "DataGenerator.h"

DataGenerator::DataGenerator(QObject *parent) :
    QObject(parent),
    m_lambda(1.0),
    m_distribution(m_lambda)
{
    std::random_device rd;
    m_generator.seed(rd());
}

void DataGenerator::setLambda(double lambda) {
    m_lambda = lambda;
    m_distribution = std::poisson_distribution<int>(m_lambda);
}

void DataGenerator::setSenders(const std::vector<QSharedPointer<PC>> &senders) {
    m_senders = senders;
    qDebug() << "DataGenerator: Set" << m_senders.size() << "senders.";
}

std::vector<int> DataGenerator::generatePoissonLoads(int numSamples, int timeScale) {
    std::vector<int> loads(timeScale, 0);
    for (int i = 0; i < numSamples; ++i) {
        int value = m_distribution(m_generator);
        if (value < timeScale) {
            loads[value]++;
        }
    }
    return loads;
}

void DataGenerator::generatePackets() {
    if (m_senders.empty()) {
        qWarning() << "DataGenerator: No senders set. Cannot generate packets.";
        return;
    }

    const int numSamples = m_packetsPerSimulation;
    const int timeScale = 100;
    std::vector<int> loads = generatePoissonLoads(numSamples, timeScale);
    std::vector<QSharedPointer<Packet>> packets;

    QMap<QString, QMap<QString, int>> distributionMap; // Origin -> (Destination -> Count)

    for (int second = 0; second < timeScale; ++second) {
        int packetsForThisSecond = loads[second];

        for (int i = 0; i < packetsForThisSecond; ++i) {
            int senderIndex = m_generator() % m_senders.size();
            QSharedPointer<PC> sender = m_senders[senderIndex];

            std::vector<QString> possibleDestinations;
            for (const auto &pc : m_senders) {
                if (pc->getId() != sender->getId()) {
                    possibleDestinations.push_back(pc->getIpAddress());
                }
            }

            if (possibleDestinations.empty()) {
                qWarning() << "DataGenerator: No valid destinations available.";
                continue;
            }

            int destIndex = m_generator() % possibleDestinations.size();
            QString destination = possibleDestinations[destIndex];

            QString actualPayload = "Hello from PC " + QString::number(sender->getId());
            QString payload = "Data:" + destination + ":" + actualPayload;

            QSharedPointer<Packet> packet = QSharedPointer<Packet>::create(PacketType::Data, payload, 64);
            packet->addToPath(sender->getIpAddress());
            packet->addToPathTaken(sender->getIpAddress());
            packet->addToPath(destination);
            packets.push_back(packet);

            distributionMap[sender->getIpAddress()][destination]++;
        }
    }

    emit packetsGenerated(packets);

    qDebug() << packets.size() << "packets generated and emitted over a timescale of" << timeScale << "seconds.";

    QTimer::singleShot(4000, this, [distributionMap]() {
        qDebug() << "---- Packet Distribution ----";
        for (auto itOrigin = distributionMap.constBegin(); itOrigin != distributionMap.constEnd(); ++itOrigin) {
            QString origin = itOrigin.key();
            for (auto itDest = itOrigin.value().constBegin(); itDest != itOrigin.value().constEnd(); ++itDest) {
                QString destination = itDest.key();
                int count = itDest.value();
                qDebug() << "Origin:" << origin << "-> Destination:" << destination << " | Packets:" << count;
            }
        }
        qDebug() << "------------------------------";
    });
}

std::vector<QSharedPointer<PC>> DataGenerator::getSenders() const {
    return m_senders;
}

void DataGenerator::loadConfig(const QString &configFilePath)
{
    QFile file(configFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "DataGenerator: Could not open config file:" << configFilePath;
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "DataGenerator: Invalid JSON in config file.";
        return;
    }

    QJsonObject rootObj = doc.object();

    if (rootObj.contains("packets_per_simulation") && rootObj["packets_per_simulation"].isDouble()) {
        m_packetsPerSimulation = rootObj["packets_per_simulation"].toInt();
        qDebug() << "DataGenerator: Loaded packets_per_simulation =" << m_packetsPerSimulation;
    } else {
        qWarning() << "DataGenerator: 'packets_per_simulation' not found or invalid in config file. Using default value."
                   << m_packetsPerSimulation;
    }
}
