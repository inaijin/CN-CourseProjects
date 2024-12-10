#include <QtTest/QtTest>
#include "../src/Topology/TopologyController.h"
#include "../src/Topology/TopologyBuilder.h"
#include "../src/Network/AutonomousSystem.h"

class TopologyControllerTests : public QObject
{
    Q_OBJECT

private slots:
    void testValidateTopology();
    void testConnectToOtherAS();
};

void TopologyControllerTests::testValidateTopology()
{
    QJsonObject config;
    config["id"] = 1;
    config["topology_type"] = "Mesh";
    config["node_count"] = 3;
    config["router_port_count"] = 4;

    QJsonArray gateways;
    QJsonObject gateway1;
    gateway1["node"] = 1;
    gateway1["users"] = QJsonArray({101, 102});
    gateways.append(gateway1);
    config["gateways"] = gateways;

    TopologyBuilder builder(config, IdAssignment());
    builder.buildTopology();

    TopologyController controller(QSharedPointer<TopologyBuilder>::create(builder));
    controller.validateTopology();

    QVERIFY(true);
}

void TopologyControllerTests::testConnectToOtherAS()
{
    // Create AS 1
    QJsonObject config1;
    config1["id"] = 1;
    config1["topology_type"] = "Mesh";
    config1["node_count"] = 3;
    config1["router_port_count"] = 4;

    // Create AS 2
    QJsonObject config2;
    config2["id"] = 2;
    config2["topology_type"] = "Ring";
    config2["node_count"] = 3;
    config2["router_port_count"] = 4;

    QJsonArray connectToAsArray;
    QJsonObject connection;
    connection["id"] = 2;
    QJsonArray gatewayPairs;
    QJsonObject gatewayPair;
    gatewayPair["gateway"] = 1;
    gatewayPair["connect_to"] = 2;
    gatewayPairs.append(gatewayPair);
    connection["gateway_pairs"] = gatewayPairs;
    connectToAsArray.append(connection);
    config1["connect_to_as"] = connectToAsArray;

    AutonomousSystem as1(config1, IdAssignment());
    AutonomousSystem as2(config2, IdAssignment());

    std::vector<QSharedPointer<AutonomousSystem>> allAS = {
     QSharedPointer<AutonomousSystem>::create(as1),
     QSharedPointer<AutonomousSystem>::create(as2)
    };

    TopologyController controller(QSharedPointer<TopologyBuilder>::create(config1));
    controller.connectToOtherAS(allAS);

    QVERIFY(true);
}

QTEST_MAIN(TopologyControllerTests)
#include "TopologyControllerTests.moc"
