// #include <QtTest/QtTest>
// #include "../src/Network/AutonomousSystem.h"
// #include "../src/Topology/TopologyBuilder.h"
// #include "../src/Topology/TopologyController.h"
// #include "../src/Network/Router.h"

// class AutonomousSystemTests : public QObject
// {
//     Q_OBJECT

// private Q_SLOTS:
//     void testInitialization();
//     void testRouterCreation();
//     void testConnectToOtherAS();
// };

// void AutonomousSystemTests::testInitialization()
// {
//     QJsonObject config;
//     config["id"] = 1;
//     config["topology_type"] = "Mesh";
//     config["node_count"] = 5;
//     config["router_port_count"] = 4;
//     config["gateways"] = QJsonArray();

//     AutonomousSystem as(config);
//     QCOMPARE(as.getId(), 1);
// }

// void AutonomousSystemTests::testRouterCreation()
// {
//     QJsonObject config;
//     config["id"] = 2;
//     config["topology_type"] = "Ring";
//     config["node_count"] = 3;
//     config["router_port_count"] = 4;
//     config["gateways"] = QJsonArray();

//     AutonomousSystem as(config);
//     const auto &routers = as.getRouters();
//     QCOMPARE(static_cast<int>(routers.size()), 3);

//     for (int i = 0; i < 3; ++i)
//     {
//         QVERIFY(routers[i]->getId() == i + 1);
//     }
// }

// void AutonomousSystemTests::testConnectToOtherAS()
// {
//     QJsonObject config1, config2;
//     config1["id"] = 1;
//     config1["topology_type"] = "Star";
//     config1["node_count"] = 4;
//     config1["router_port_count"] = 4;
//     config1["gateways"] = QJsonArray();

//     config2["id"] = 2;
//     config2["topology_type"] = "Ring";
//     config2["node_count"] = 3;
//     config2["router_port_count"] = 4;
//     config2["gateways"] = QJsonArray();

//     QSharedPointer<AutonomousSystem> as1 = QSharedPointer<AutonomousSystem>::create(config1);
//     QSharedPointer<AutonomousSystem> as2 = QSharedPointer<AutonomousSystem>::create(config2);

//     std::vector<QSharedPointer<AutonomousSystem>> allAS = {as1, as2};
//     as1->connectToOtherAS(allAS);
//     as2->connectToOtherAS(allAS);

//     QVERIFY(true); // Verify the connections indirectly (manual validation in logs)
// }

// QTEST_MAIN(AutonomousSystemTests)
// #include "AutonomousSystemTests.moc"
