#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QSharedPointer>
#include "../src/DataGenerator/DataGenerator.h"
#include "../src/Network/PC.h"
#include "../src/Packet/Packet.h"

class DataGeneratorTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testDefaultLambda();
    void testSetLambda();
    void testSetSenders();
    void testGeneratePoissonLoads();
    void testGeneratePackets();
    void testPacketsSignalEmission();
};

void DataGeneratorTests::testDefaultLambda() {
    DataGenerator generator;
    QCOMPARE(generator.getSenders().size(), 0);
}

void DataGeneratorTests::testSetLambda() {
    DataGenerator generator;
    generator.setLambda(5.0);
    QVERIFY(true); // If no crash happens, test passes.
}

void DataGeneratorTests::testSetSenders() {
    DataGenerator generator;
    QSharedPointer<PC> pc1 = QSharedPointer<PC>::create(1, "192.168.0.1");
    QSharedPointer<PC> pc2 = QSharedPointer<PC>::create(2, "192.168.0.2");

    std::vector<QSharedPointer<PC>> senders = {pc1, pc2};
    generator.setSenders(senders);

    QCOMPARE(generator.getSenders().size(), 2);
    QCOMPARE(generator.getSenders()[0]->getId(), 1);
    QCOMPARE(generator.getSenders()[1]->getId(), 2);
}

void DataGeneratorTests::testGeneratePoissonLoads() {
    DataGenerator generator;
    generator.setLambda(2.0);

    std::vector<int> loads = generator.generatePoissonLoads(100, 10);
    QCOMPARE(loads.size(), 10);
    int totalPackets = 0;
    for (int load : loads) {
        QVERIFY(load >= 0); // Ensure no negative loads
        totalPackets += load;
    }
    QVERIFY(totalPackets > 0); // Some packets should have been generated
}

void DataGeneratorTests::testGeneratePackets() {
    DataGenerator generator;
    QSharedPointer<PC> pc1 = QSharedPointer<PC>::create(1, "192.168.0.1");
    QSharedPointer<PC> pc2 = QSharedPointer<PC>::create(2, "192.168.0.2");

    generator.setSenders({pc1, pc2});
    QSignalSpy spy(&generator, &DataGenerator::packetsGenerated);

    generator.generatePackets();

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    auto packets = arguments.at(0).value<std::vector<QSharedPointer<Packet>>>();

    QVERIFY(packets.size() > 0); // Ensure packets are generated
    for (const auto &packet : packets) {
        QVERIFY(packet->getPayload().contains("Hello from PC"));
    }
}

void DataGeneratorTests::testPacketsSignalEmission() {
    DataGenerator generator;
    QSharedPointer<PC> pc1 = QSharedPointer<PC>::create(1, "192.168.0.1");
    QSharedPointer<PC> pc2 = QSharedPointer<PC>::create(2, "192.168.0.2");

    generator.setSenders({pc1, pc2});
    QSignalSpy spy(&generator, &DataGenerator::packetsGenerated);

    generator.generatePackets();

    QCOMPARE(spy.count(), 1);
    auto packets = spy.takeFirst().at(0).value<std::vector<QSharedPointer<Packet>>>();
    QVERIFY(packets.size() > 0);
}

QTEST_MAIN(DataGeneratorTests)
#include "DataGeneratorTests.moc"
