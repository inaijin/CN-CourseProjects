// #include <QtTest/QtTest>
// #include "../src/Topology/TopologyController.h"
// #include "../src/Topology/TopologyBuilder.h"
// #include "../src/Network/AutonomousSystem.h"

// class TopologyControllerTest : public QObject
// {
//     Q_OBJECT

// private Q_SLOTS:
//     void testValidateTopology();
//     void testConnectToOtherAS();
// };

// void TopologyControllerTest::testValidateTopology()
// {
//     QJsonObject config;
//     config["id"] = 1;
//     config["topology_type"] = "Mesh";
//     config["node_count"] = 3;
//     config["router_port_count"] = 4;

//     QJsonArray gateways;
//     QJsonObject gateway1;
//     gateway1["node"] = 1;
//     gateway1["users"] = QJsonArray({101, 102});
//     gateways.append(gateway1);
//     config["gateways"] = gateways;

//     QSharedPointer<TopologyBuilder> builder = QSharedPointer<TopologyBuilder>::create(config);
//     builder->buildTopology();

//     TopologyController controller(builder);
//     controller.validateTopology();

//     QVERIFY(true);
// }

// void TopologyControllerTest::testConnectToOtherAS()
// {
//     // Create AS 1
//     QJsonObject config1;
//     config1["id"] = 1;
//     config1["topology_type"] = "Mesh";
//     config1["node_count"] = 3;
//     config1["router_port_count"] = 4;

//     QSharedPointer<AutonomousSystem> as1 = QSharedPointer<AutonomousSystem>::create(config1);

//     // Create AS 2
//     QJsonObject config2;
//     config2["id"] = 2;
//     config2["topology_type"] = "Ring";
//     config2["node_count"] = 3;
//     config2["router_port_count"] = 4;

//     QJsonArray connectToAsArray;
//     QJsonObject connection;
//     connection["id"] = 2;
//     QJsonArray gatewayPairs;
//     QJsonObject gatewayPair;
//     gatewayPair["gateway"] = 1;
//     gatewayPair["connect_to"] = 2;
//     gatewayPairs.append(gatewayPair);
//     connection["gateway_pairs"] = gatewayPairs;
//     connectToAsArray.append(connection);
//     config1["connect_to_as"] = connectToAsArray;

//     QSharedPointer<AutonomousSystem> as2 = QSharedPointer<AutonomousSystem>::create(config2);

//     QSharedPointer<TopologyBuilder> builder1 = QSharedPointer<TopologyBuilder>::create(config1);
//     builder1->buildTopology();

//     QSharedPointer<TopologyBuilder> builder2 = QSharedPointer<TopologyBuilder>::create(config2);
//     builder2->buildTopology();

//     std::vector<QSharedPointer<AutonomousSystem>> allAS = {as1, as2};

//     TopologyController controller1(builder1);
//     controller1.connectToOtherAS(allAS);

//     QVERIFY(true);
// }

// // QTEST_MAIN(TopologyControllerTest)
// #include "TopologyControllerTests.moc"
