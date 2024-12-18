#ifndef TOPOLOGYCONTROLLER_H
#define TOPOLOGYCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include <vector>
#include <../Network/Router.h>

class TopologyBuilder;
class AutonomousSystem;

class TopologyController : public QObject
{
    Q_OBJECT

public:
    explicit TopologyController(const QSharedPointer<TopologyBuilder> &builder, QObject *parent = nullptr);
    ~TopologyController();

    void validateTopology() const;
    void connectToOtherAS(const std::vector<QSharedPointer<AutonomousSystem>> &allAS);
    void initiateDHCPPhase();
    void checkAssignedIP();
    void checkAssignedIPPC();

private:
    QSharedPointer<TopologyBuilder> m_builder;
    void initiateDHCPIP(const QSharedPointer<Router> &router);
};

#endif // TOPOLOGYCONTROLLER_H
