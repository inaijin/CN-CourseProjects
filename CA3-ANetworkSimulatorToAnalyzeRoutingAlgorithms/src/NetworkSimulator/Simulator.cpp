#include "Simulator.h"
#include <QFile>
#include <QJsonArray>
#include <QDebug>

Simulator::Simulator(QObject *parent)
    : QObject(parent)
{
}

Simulator::~Simulator()
{
    // Cleanup if necessary
}

bool Simulator::loadConfig(const QString &configFilePath)
{
    QFile configFile(configFilePath);
    if (!configFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open config file:" << configFilePath;
        return false;
    }

    QByteArray data = configFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
    {
        qWarning() << "Failed to parse config file.";
        return false;
    }

    m_config = doc.object();
    return true;
}

void Simulator::initializeNetwork()
{
    createAutonomousSystems();
    connectAutonomousSystems();
}

void Simulator::createAutonomousSystems()
{
    QJsonArray asArray = m_config.value("Autonomous_systems").toArray();
    for (const QJsonValue &asValue : asArray)
    {
        QJsonObject asObject = asValue.toObject();
        auto asInstance = QSharedPointer<AutonomousSystem>::create(asObject);
        m_autonomousSystems.push_back(asInstance);
    }
}

void Simulator::connectAutonomousSystems()
{
    for (const auto &asInstance : m_autonomousSystems)
    {
        asInstance->connectToOtherAS(m_autonomousSystems);
    }
}

void Simulator::startSimulation()
{
    // For Phase 1, routers remain inactive, Implement in future phases
    qDebug() << "Simulation initialized. Network topology is set up.";
}
