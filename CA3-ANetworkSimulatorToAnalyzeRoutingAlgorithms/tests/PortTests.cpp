#include <QtTest/QtTest>
#include "../src/Port/Port.h"

class PortTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testSetAndGetPortNumber();
    void testSetAndGetRouterIP();
    void testConnectionState();
    void testPacketTransmission();
};

void PortTests::testSetAndGetPortNumber() {
    Port port;
    port.setPortNumber(10);
    QCOMPARE(port.getPortNumber(), static_cast<uint8_t>(10));
}

void PortTests::testSetAndGetRouterIP() {
    Port port;
    QString ip = "192.168.1.1";
    port.setRouterIP(ip);
    QCOMPARE(port.getRouterIP(), ip);
}

void PortTests::testConnectionState() {
    Port port;
    QVERIFY(!port.isConnected());
    port.setConnected(true);
    QVERIFY(port.isConnected());
}

void PortTests::testPacketTransmission() {
    Port port1, port2;

    QSignalSpy spy1(&port1, &Port::packetSent);
    QSignalSpy spy2(&port2, &Port::packetReceived);

    connect(&port1, &Port::packetSent, &port2, &Port::receivePacket);

    auto packet = QSharedPointer<Packet>::create(PacketType::Data, "TestPayload");
    port1.sendPacket(packet);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
}

QTEST_MAIN(PortTests)
#include "PortTests.moc"
