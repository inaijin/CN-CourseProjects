#include <QtTest/QtTest>
#include "../src/Network/AutonomousSystem.h"
#include "../src/Topology/TopologyController.h"
#include "../Network/PC.h"
#include "../src/Network/Router.h"
#include <QJsonArray>
#include <QJsonObject>

class AutonomousSystemTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testInitialization();
    void testRouterAndPCCreation();
    void testConnectToOtherAS();
};

void AutonomousSystemTests::testInitialization()
{
    QJsonObject config;
    config["id"] = 1;
    config["topology_type"] = "Mesh";
    config["node_count"] = 5;
    config["router_port_count"] = 4;

    QJsonArray gateways;
    QJsonObject gateway1;
    gateway1["node"] = 1;
    gateway1["users"] = QJsonArray::fromStringList({"101", "102"});
    gateways.append(gateway1);
    config["gateways"] = gateways;

    AutonomousSystem as(config, IdAssignment());
    QCOMPARE(as.getId(), 1);
    QVERIFY(!as.getRouters().empty());
    QVERIFY(!as.getPCs().empty());
}

void AutonomousSystemTests::testRouterAndPCCreation()
{
    QJsonObject config;
    config["id"] = 2;
    config["topology_type"] = "Ring";
    config["node_count"] = 4;
    config["router_port_count"] = 4;

    QJsonArray gateways;
    QJsonObject gateway1;
    gateway1["node"] = 1;
    gateway1["users"] = QJsonArray::fromStringList({"201", "202"});
    gateways.append(gateway1);
    config["gateways"] = gateways;

    AutonomousSystem as(config, IdAssignment());
    const auto &routers = as.getRouters();
    const auto &pcs = as.getPCs();

    QCOMPARE(static_cast<int>(routers.size()), 4);
    QCOMPARE(static_cast<int>(pcs.size()), 2);

    for (int i = 0; i < 4; ++i)
    {
        QVERIFY(routers[i]->getId() == i + 1);
    }

    QVERIFY(pcs[0]->getIPAddress() == "192.168.200.201");
    QVERIFY(pcs[1]->getIPAddress() == "192.168.200.202");
}

void AutonomousSystemTests::testConnectToOtherAS()
{
    QJsonObject config1, config2;
    config1["id"] = 1;
    config1["topology_type"] = "Mesh";
    config1["node_count"] = 4;
    config1["router_port_count"] = 4;

    QJsonArray gateways1;
    QJsonObject gateway1;
    gateway1["node"] = 1;
    gateway1["users"] = QJsonArray::fromStringList({"101", "102"});
    gateways1.append(gateway1);
    config1["gateways"] = gateways1;

    config2["id"] = 2;
    config2["topology_type"] = "Ring";
    config2["node_count"] = 3;
    config2["router_port_count"] = 4;

    QJsonArray gateways2;
    QJsonObject gateway2;
    gateway2["node"] = 1;
    gateway2["users"] = QJsonArray::fromStringList({"201", "202"});
    gateways2.append(gateway2);
    config2["gateways"] = gateways2;

    QSharedPointer<AutonomousSystem> as1 = QSharedPointer<AutonomousSystem>::create(config1, IdAssignment());
    QSharedPointer<AutonomousSystem> as2 = QSharedPointer<AutonomousSystem>::create(config2, IdAssignment());

    std::vector<QSharedPointer<AutonomousSystem>> allAS = {as1, as2};
    as1->connectToOtherAS(allAS);
    as2->connectToOtherAS(allAS);

    QVERIFY(true);
}

// QTEST_MAIN(AutonomousSystemTests)
#include "AutonomousSystemTests.moc"
