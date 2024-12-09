#include <QtTest/QtTest>
#include "../src/Topology/TopologyBuilder.h"
#include "../src/Network/Router.h"
#include "../src/Network/PC.h"

class TopologyBuilderTests : public QObject
{
    Q_OBJECT

private slots:
    void testBuildTopology();
    void testCreateRouters();
    void testCreatePCs();
};

void TopologyBuilderTests::testBuildTopology()
{
    QJsonObject config;
    config.insert("id", 1);
    config.insert("node_count", 3);
    config.insert("router_port_count", 6);
    config.insert("topology_type", "Mesh");

    QJsonArray gateways;
    QJsonObject gateway;
    gateway.insert("node", 1);
    QJsonArray users;
    users.append(101);
    users.append(102);
    gateway.insert("users", users);
    gateways.append(gateway);
    config.insert("gateways", gateways);

    TopologyBuilder builder(config);
    builder.buildTopology();

    QCOMPARE(static_cast<int>(builder.getRouters().size()), 3);
    QCOMPARE(builder.getPCs().size(), 2);
}

void TopologyBuilderTests::testCreateRouters()
{
    QJsonObject config;
    config.insert("id", 1);
    config.insert("node_count", 2);
    config.insert("router_port_count", 4);

    TopologyBuilder builder(config);
    builder.buildTopology();

    QCOMPARE(builder.getRouters().size(), 2);
    QVERIFY(builder.getRouters()[0]->getId() == 1);
}

void TopologyBuilderTests::testCreatePCs()
{
    QJsonObject config;
    config.insert("id", 1);

    QJsonArray gateways;
    QJsonObject gateway;
    gateway.insert("node", 1);
    QJsonArray users;
    users.append(201);
    users.append(202);
    gateway.insert("users", users);
    gateways.append(gateway);
    config.insert("gateways", gateways);

    TopologyBuilder builder(config);
    builder.buildTopology();

    QCOMPARE(builder.getPCs().size(), 2);
    QCOMPARE(builder.getPCs()[0]->getId(), 201);
}

QTEST_MAIN(TopologyBuilderTests)
#include "TopologyBuilderTests.moc"
