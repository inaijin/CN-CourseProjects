// #include <QtTest/QtTest>
// #include "../src/Topology/TopologyBuilder.h"
// #include "../src/Network/Router.h"
// #include "../src/Network/PC.h"

// class TopologyBuilderTests : public QObject
// {
//     Q_OBJECT

// private slots:
//     void testBuildTopology();
//     void testCreateRouters();
//     void testCreatePCs();
// };

// void TopologyBuilderTests::testBuildTopology()
// {
//     QJsonObject config;
//     config.insert("id", 1);
//     config.insert("node_count", 3);
//     config.insert("router_port_count", 6);
//     config.insert("topology_type", "Mesh");

//     QJsonArray gateways;
//     QJsonObject gateway;
//     gateway.insert("node", 1);
//     QJsonArray users;
//     users.append(101);
//     users.append(102);
//     gateway.insert("users", users);
//     gateways.append(gateway);
//     config.insert("gateways", gateways);

//     TopologyBuilder builder(config);
//     builder.buildTopology();

//     QCOMPARE(static_cast<int>(builder.getRouters().size()), 3);
//     QCOMPARE(static_cast<int>(builder.getPCs().size()), 2);
// }

// void TopologyBuilderTests::testCreateRouters()
// {
//     QJsonObject config;
//     config.insert("id", 1);
//     config.insert("node_count", 2);
//     config.insert("router_port_count", 4);

//     TopologyBuilder builder(config);
//     builder.buildTopology();

//     QCOMPARE(static_cast<int>(builder.getRouters().size()), 2);
//     QVERIFY(builder.getRouters()[0]->getId() == 1);
// }

// void TopologyBuilderTests::testCreatePCs()
// {
//     QJsonObject config;
//     config.insert("id", 1);
//     config.insert("node_count", 3);

//     QJsonArray gateways;
//     QJsonObject gateway;
//     gateway.insert("node", 1);
//     QJsonArray users;
//     users.append(201);
//     users.append(202);
//     gateway.insert("users", users);
//     gateways.append(gateway);
//     config.insert("gateways", gateways);

//     try {
//         TopologyBuilder builder(config);
//         builder.buildTopology();

//         QCOMPARE(static_cast<int>(builder.getPCs().size()), 2);
//         QCOMPARE(builder.getPCs()[0]->getId(), 201);
//         QCOMPARE(builder.getPCs()[1]->getId(), 202);

//         auto pc1 = builder.getPCs()[0];
//         auto pc2 = builder.getPCs()[1];

//         QVERIFY(pc1);
//         QVERIFY(pc2);

//         QCOMPARE(pc1->getIPAddress(), QString("192.168.100.201"));
//         QCOMPARE(pc2->getIPAddress(), QString("192.168.100.202"));

//         QVERIFY(pc1->getPort());
//         QVERIFY(pc1->getPort()->isConnected());

//         QVERIFY(pc2->getPort());
//         QVERIFY(pc2->getPort()->isConnected());
//     } catch (const std::exception &e) {
//         QFAIL(qPrintable(QString("Caught unexpected exception: %1").arg(e.what())));
//     }
// }

// // QTEST_MAIN(TopologyBuilderTests)
// #include "TopologyBuilderTests.moc"
