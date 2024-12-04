#include <QtTest/QtTest>
#include "../src/Header/DataLinkHeader.h"
#include "../src/MACAddress/MACAddress.h"

class DataLinkHeaderTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testDefaultConstructor();
    void testParameterizedConstructor();
    void testSourceMACManagement();
    void testDestinationMACManagement();
    void testFrameTypeManagement();
    void testErrorDetectionCodeManagement();
};

void DataLinkHeaderTests::testDefaultConstructor() {
    DataLinkHeader header;

    QCOMPARE(header.getSourceMAC().toString(), QString("00:00:00:00:00:00"));
    QCOMPARE(header.getDestinationMAC().toString(), QString("00:00:00:00:00:00"));
    QCOMPARE(header.getFrameType(), QString("0x0800"));
    QCOMPARE(header.getErrorDetectionCode(), QString("0000"));
}

void DataLinkHeaderTests::testParameterizedConstructor() {
    MACAddress sourceMAC("12:34:56:78:9A:BC");
    MACAddress destinationMAC("AB:CD:EF:01:23:45");
    DataLinkHeader header(sourceMAC, destinationMAC, "0x0806", "1111");

    QCOMPARE(header.getSourceMAC().toString(), QString("12:34:56:78:9A:BC"));
    QCOMPARE(header.getDestinationMAC().toString(), QString("AB:CD:EF:01:23:45"));
    QCOMPARE(header.getFrameType(), QString("0x0806"));
    QCOMPARE(header.getErrorDetectionCode(), QString("1111"));
}

void DataLinkHeaderTests::testSourceMACManagement() {
    DataLinkHeader header;

    MACAddress newSourceMAC("AA:BB:CC:DD:EE:FF");
    header.setSourceMAC(newSourceMAC);

    QCOMPARE(header.getSourceMAC().toString(), QString("AA:BB:CC:DD:EE:FF"));
}

void DataLinkHeaderTests::testDestinationMACManagement() {
    DataLinkHeader header;

    MACAddress newDestinationMAC("FF:EE:DD:CC:BB:AA");
    header.setDestinationMAC(newDestinationMAC);

    QCOMPARE(header.getDestinationMAC().toString(), QString("FF:EE:DD:CC:BB:AA"));
}

void DataLinkHeaderTests::testFrameTypeManagement() {
    DataLinkHeader header;

    header.setFrameType("0x0805");
    QCOMPARE(header.getFrameType(), QString("0x0805"));
}

void DataLinkHeaderTests::testErrorDetectionCodeManagement() {
    DataLinkHeader header;

    header.setErrorDetectionCode("ABCD");
    QCOMPARE(header.getErrorDetectionCode(), QString("ABCD"));
}

QTEST_MAIN(DataLinkHeaderTests)
#include "DataLinkHeaderTests.moc"
