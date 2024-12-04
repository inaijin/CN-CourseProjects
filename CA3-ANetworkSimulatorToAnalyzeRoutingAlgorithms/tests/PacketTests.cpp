#include <QtTest/QtTest>
#include "../src/Packet/Packet.h"

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

QTEST_MAIN(PacketTests)
#include "PacketTests.moc"
