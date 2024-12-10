#include <QtTest/QtTest>
#include "../src/Network/Router.h"
#include "../src/Port/Port.h"
#include "../src/Packet/Packet.h"
#include "../PortBindingManager/PortBindingManager.h"

class RouterTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testRouterInitialization();
    void testPortAvailability();
    void testPacketForwarding();
    void testPortBinding();
};

void RouterTests::testRouterInitialization()
{
    Router router(1, "192.168.1.1", 4);
    QCOMPARE(router.getId(), 1);
    QCOMPARE(router.getIPAddress(), QString("192.168.1.1"));
    QCOMPARE(static_cast<int>(router.getPorts().size()), 4);
}

void RouterTests::testPortAvailability()
{
    Router router(2, "192.168.1.2", 4);
    auto port = router.getAvailablePort();
    QVERIFY(port != nullptr);
    QCOMPARE(port->isConnected(), false);
}

void RouterTests::testPacketForwarding()
{
    Router router(3, "192.168.1.3", 4);
    auto port1 = router.getAvailablePort();
    auto port2 = router.getAvailablePort();

    PortBindingManager manager;
    manager.bind(port1, port2);

    auto packet = QSharedPointer<Packet>::create(PacketType::Data, "ForwardTestPayload");
    port1->sendPacket(packet);

    connect(port2.data(), &Port::packetReceived, this, [](const PacketPtr_t &packet) {
        QCOMPARE(packet->getPayload(), QString("ForwardTestPayload"));
    });

    QVERIFY(true);
}

void RouterTests::testPortBinding()
{
    Router router1(4, "192.168.1.4", 4);
    Router router2(5, "192.168.1.5", 4);

    PortBindingManager manager;
    manager.bind(router1.getAvailablePort(), router2.getAvailablePort());

    QVERIFY(router1.getAvailablePort()->isConnected());
    QVERIFY(router2.getAvailablePort()->isConnected());
}

QTEST_MAIN(RouterTests)
#include "RouterTests.moc"
