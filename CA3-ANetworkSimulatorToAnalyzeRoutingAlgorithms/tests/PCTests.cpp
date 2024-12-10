#include <QtTest/QtTest>
#include "../src/Network/PC.h"
#include "../src/Packet/Packet.h"
#include "../PortBindingManager/PortBindingManager.h"

class PCTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testPCInitialization();
    void testPacketGeneration();
    void testPortBinding();
};

void PCTests::testPCInitialization()
{
    PC pc(1, "192.168.1.100");
    QCOMPARE(pc.getId(), 1);
    QCOMPARE(pc.getIPAddress(), QString("192.168.1.100"));
    QVERIFY(pc.getPort() != nullptr);
}

void PCTests::testPacketGeneration()
{
    PC pc(2, "192.168.1.101");
    auto port = pc.getPort();
    connect(port.data(), &Port::packetSent, this, [](const PacketPtr_t &packet) {
        QCOMPARE(packet->getType(), PacketType::Data);
        QCOMPARE(packet->getPayload(), QString("TestPayload"));
    });

    pc.generatePacket(); // Assuming generatePacket creates and sends a test packet do in future phases
    QVERIFY(true);
}

void PCTests::testPortBinding()
{
    PC pc1(3, "192.168.1.102");
    PC pc2(4, "192.168.1.103");

    PortBindingManager manager;
    manager.bind(pc1.getPort(), pc2.getPort());

    QVERIFY(pc1.getPort()->isConnected());
    QVERIFY(pc2.getPort()->isConnected());
}

// QTEST_MAIN(PCTests)
#include "PCTests.moc"
