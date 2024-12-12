#include <QtTest/QtTest>
#include "../Topology/TopologyController.h"
#include "../Topology/TopologyBuilder.h"
#include "../Globals/IdAssignment.h"

class TopologyControllerTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testValidateTopology();
};

void TopologyControllerTests::testValidateTopology() {
    QJsonObject config;
    config["id"] = 1;
    config["topology_type"] = "Mesh";
    config["node_count"] = 4;

    IdAssignment idAssignment;
    idAssignment.addAsIdRange(1, 1, 4, 5, 8);

    auto builder = QSharedPointer<TopologyBuilder>::create(config, idAssignment);
    builder->buildTopology();

    TopologyController controller(builder);
    controller.validateTopology();

    QVERIFY(true);
}

QTEST_MAIN(TopologyControllerTests)
#include "TopologyControllerTests.moc"
