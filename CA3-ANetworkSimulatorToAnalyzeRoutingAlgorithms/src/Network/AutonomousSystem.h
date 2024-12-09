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
    ~AutonomousSystem() override;

    int getId() const;
    const std::vector<QSharedPointer<Router>> &getRouters() const;
    const std::vector<QSharedPointer<PC>> &getPCs() const;
    void connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS);

    void createRouters();
    void createPCs();
    void setupTopology();

private:
    int m_id;
    QString m_topologyType;
    QJsonObject m_config;

    QSharedPointer<TopologyBuilder> m_topologyBuilder;
    QSharedPointer<TopologyController> m_topologyController;
};

#endif // AUTONOMOUSSYSTEM_H
