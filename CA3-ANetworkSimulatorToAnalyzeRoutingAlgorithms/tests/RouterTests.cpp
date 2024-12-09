#include <QtTest/QtTest>
#include "../src/Network/Router.h"
#include "../src/Port/Port.h"

class RouterTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testRouterInitialization();
    void testPortAvailability();
    void testLogPortStatuses();
    void testPacketForwarding(); // Placeholder for future phases implementation
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

void RouterTests::testLogPortStatuses()
{
    Router router(3, "192.168.1.3", 3);
    router.logPortStatuses();
}

void RouterTests::testPacketForwarding()
{
    // Placeholder for packet forwarding test in future phases
    QVERIFY(true);
}

QTEST_MAIN(RouterTests)
#include "RouterTests.moc"
