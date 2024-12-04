#include <QtTest/QtTest>
#include "../src/Packet/Packet.h"
#include "../src/Header/DataLinkHeader.h"
#include "../src/Header/TCPHeader.h"

class PacketTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testDefaultConstructor();
    void testParameterizedConstructor();
    void testPayloadManagement();
    void testPathManagement();
    void testWaitCyclesManagement();
    void testQueueWaitCyclesManagement();
    void testSequenceNumber();
    void testDroppedFlag();
    void testDataLinkHeaderIntegration();
    void testTCPHeaderIntegration();
};

void PacketTests::testDefaultConstructor() {
    Packet packet;
    QCOMPARE(packet.getType(), PacketType::Data);
    QCOMPARE(packet.getPayload(), QString(""));
    QCOMPARE(packet.getWaitCycles(), 0);
    QCOMPARE(packet.getQueueWaitCycles(), 0);
    QCOMPARE(packet.isDropped(), false);
}

void PacketTests::testParameterizedConstructor() {
    Packet packet(PacketType::Control, "TestPayload");
    QCOMPARE(packet.getType(), PacketType::Control);
    QCOMPARE(packet.getPayload(), QString("TestPayload"));
}

void PacketTests::testPayloadManagement() {
    Packet packet;
    packet.setPayload("NewPayload");
    QCOMPARE(packet.getPayload(), QString("NewPayload"));
}

void PacketTests::testPathManagement() {
    Packet packet;
    packet.addToPath("192.168.1.1");
    packet.addToPath("192.168.1.2");

    QVector<QString> path = packet.getPath();
    QCOMPARE(path.size(), 2);
    QCOMPARE(path[0], QString("192.168.1.1"));
    QCOMPARE(path[1], QString("192.168.1.2"));
}

void PacketTests::testWaitCyclesManagement() {
    Packet packet;
    packet.incrementWaitCycles();
    packet.incrementWaitCycles();
    QCOMPARE(packet.getWaitCycles(), 2);
}

void PacketTests::testQueueWaitCyclesManagement() {
    Packet packet;
    packet.incrementQueueWaitCycles();
    packet.incrementQueueWaitCycles();
    packet.incrementQueueWaitCycles();
    QCOMPARE(packet.getQueueWaitCycles(), 3);
}

void PacketTests::testSequenceNumber() {
    Packet packet;
    packet.setSequenceNumber(42);
    QCOMPARE(packet.getSequenceNumber(), 42);
}

void PacketTests::testDroppedFlag() {
    Packet packet;
    packet.markDropped(true);
    QCOMPARE(packet.isDropped(), true);

    packet.markDropped(false);
    QCOMPARE(packet.isDropped(), false);
}

void PacketTests::testDataLinkHeaderIntegration() {
    Packet packet;
    DataLinkHeader header(MACAddress("12:34:56:78:9A:BC"), MACAddress("AB:CD:EF:01:23:45"), "0x0806", "FFFF");
    packet.setDataLinkHeader(header);

    DataLinkHeader retrievedHeader = packet.getDataLinkHeader();
    QCOMPARE(retrievedHeader.getSourceMAC().toString(), QString("12:34:56:78:9A:BC"));
    QCOMPARE(retrievedHeader.getDestinationMAC().toString(), QString("AB:CD:EF:01:23:45"));
    QCOMPARE(retrievedHeader.getFrameType(), QString("0x0806"));
    QCOMPARE(retrievedHeader.getErrorDetectionCode(), QString("FFFF"));
}

void PacketTests::testTCPHeaderIntegration() {
    Packet packet;
    TCPHeader header(8080, 8443, 12345, 67890, 5, 0b101010, 1024, 0xFFFF, 123);
    packet.setTCPHeader(header);

    TCPHeader retrievedHeader = packet.getTCPHeader();
    QCOMPARE(retrievedHeader.getSourcePort(), static_cast<uint16_t>(8080));
    QCOMPARE(retrievedHeader.getDestPort(), static_cast<uint16_t>(8443));
    QCOMPARE(retrievedHeader.getSequenceNumber(), static_cast<uint32_t>(12345));
    QCOMPARE(retrievedHeader.getAcknowledgmentNumber(), static_cast<uint32_t>(67890));
    QCOMPARE(retrievedHeader.getDataOffset(), static_cast<uint8_t>(5));
    QCOMPARE(retrievedHeader.getFlags(), static_cast<uint8_t>(0b101010));
    QCOMPARE(retrievedHeader.getWindowSize(), static_cast<uint16_t>(1024));
    QCOMPARE(retrievedHeader.getChecksum(), static_cast<uint16_t>(0xFFFF));
    QCOMPARE(retrievedHeader.getUrgentPointer(), static_cast<uint16_t>(123));
}

QTEST_MAIN(PacketTests)
#include "PacketTests.moc"
