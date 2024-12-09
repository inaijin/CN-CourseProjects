#ifndef AUTONOMOUSSYSTEM_H
#define AUTONOMOUSSYSTEM_H

#include <QObject>
#include <QSharedPointer>
#include <QJsonObject>
#include <vector>

class Router;
class PC;
class TopologyBuilder;
class TopologyController;

class AutonomousSystem : public QObject
{
    Q_OBJECT

public:
    explicit AutonomousSystem(const QJsonObject &config, QObject *parent = nullptr);
    ~AutonomousSystem();

    int getId() const;
    const std::vector<QSharedPointer<Router>> &getRouters() const;

    void connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS);

private:
    int m_id;
    QJsonObject m_config;

    QSharedPointer<TopologyBuilder> m_topologyBuilder;
    QSharedPointer<TopologyController> m_topologyController;
};

#endif // AUTONOMOUSSYSTEM_H
