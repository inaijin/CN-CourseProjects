// #include <QtTest/QtTest>
// #include "../Topology/TopologyBuilder.h"

// class TopologyBuilderTests : public QObject {
//     Q_OBJECT

// private Q_SLOTS:
//     void testBuildTopology();
// };

// void TopologyBuilderTests::testBuildTopology() {
//     QJsonObject config;
//     config["id"] = 1;
//     config["topology_type"] = "Mesh";
//     config["node_count"] = 4;

//     IdAssignment idAssignment;
//     idAssignment.addAsIdRange(1, 1, 4, 5, 8);

//     TopologyBuilder builder(config, idAssignment);
//     builder.buildTopology();

//     QCOMPARE(static_cast<int>(builder.getRouters().size()), 4);
//     QVERIFY(!builder.getPCs().empty());
// }

// // QTEST_MAIN(TopologyBuilderTests)
// #include "TopologyBuilderTests.moc"


// UPDATE LATER
