#ifndef TOPOLOGYCONTROLLER_H
#define TOPOLOGYCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include <vector>

class TopologyBuilder;
class AutonomousSystem;

class TopologyController : public QObject
{
    Q_OBJECT

public:
    explicit TopologyController(const QSharedPointer<TopologyBuilder> &builder, QObject *parent = nullptr);
    ~TopologyController();

    void validateTopology();
    void connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS);

private:
    QSharedPointer<TopologyBuilder> m_builder;
};

#endif // TOPOLOGYCONTROLLER_H
