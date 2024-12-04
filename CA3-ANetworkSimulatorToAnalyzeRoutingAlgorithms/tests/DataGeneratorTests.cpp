#include <QtTest/QtTest>
#include "../src/DataGenerator/DataGenerator.h"
#include "../src/Packet/Packet.h"

class DataGeneratorTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testLambdaSetting();
    void testDestinationSetting();
    void testPacketGeneration();
};

void DataGeneratorTests::testLambdaSetting() {
    DataGenerator generator;
    generator.setLambda(5.0);

    std::vector<QString> destinations = {"192.168.1.1", "192.168.1.2"};
    generator.setDestinations(destinations);

    QSignalSpy spy(&generator, &DataGenerator::packetsGenerated);
    generator.generatePackets();

    QTest::qWait(100);
    QVERIFY(spy.count() > 0);
}

void DataGeneratorTests::testDestinationSetting() {
    DataGenerator generator;

    std::vector<QString> destinations = {"192.168.1.1", "192.168.1.2", "192.168.1.3"};
    generator.setDestinations(destinations);

    QSignalSpy spy(&generator, &DataGenerator::packetsGenerated);
    generator.generatePackets();

    QVERIFY(spy.count() > 0);

    auto packets = qvariant_cast<std::vector<QSharedPointer<Packet>>>(spy.takeFirst().at(0));
    for (const auto &packet : packets) {
        QString path = packet->getPath().last();
        QVERIFY(std::find(destinations.begin(), destinations.end(), path) != destinations.end());
    }
}

void DataGeneratorTests::testPacketGeneration() {
    DataGenerator generator;
    generator.setLambda(3.0);

    std::vector<QString> destinations = {"10.0.0.1", "10.0.0.2"};
    generator.setDestinations(destinations);

    QSignalSpy spy(&generator, &DataGenerator::packetsGenerated);
    generator.generatePackets();

    QVERIFY(spy.count() > 0);
    auto packets = qvariant_cast<std::vector<QSharedPointer<Packet>>>(spy.takeFirst().at(0));

    QVERIFY(!packets.empty());
    for (const auto &packet : packets) {
        QVERIFY(!packet->getPath().isEmpty());
        QVERIFY(packet->getPayload() == "GeneratedPayload");
    }
}

// QTEST_MAIN(DataGeneratorTests)
#include "DataGeneratorTests.moc"
