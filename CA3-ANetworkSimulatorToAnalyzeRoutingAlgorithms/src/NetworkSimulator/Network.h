#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QSharedPointer>
#include <QJsonObject>
#include <QJsonArray>
#include <vector>
#include "../Network/AutonomousSystem.h"

class Network : public QObject
{
    Q_OBJECT

public:
    explicit Network(const QJsonObject &config, QObject *parent = nullptr);
    ~Network();

    void initialize();
    void connectAutonomousSystems();

    std::vector<QSharedPointer<AutonomousSystem>> getAutonomousSystems() const;

private:
    QJsonObject m_config;
    std::vector<QSharedPointer<AutonomousSystem>> m_autonomousSystems;

    void createAutonomousSystems();
};

#endif // NETWORK_H
