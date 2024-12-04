#include "Simulator.h"
#include <QFile>
#include <QJsonDocument>
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
    m_network = QSharedPointer<Network>::create(m_config);
    m_network->initialize();
}

void Simulator::startSimulation()
{
    // For Phase 1, routers remain inactive. Implement in future phases.
    qDebug() << "Simulation initialized. Network topology is set up.";
}
