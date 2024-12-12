#include <QtTest/QtTest>
#include "../Network/Router.h"
#include "../DHCPServer/DHCPServer.h"
#include "../EventsCoordinator/EventsCoordinator.h"

class RouterDHCPIntegrationTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testRouterReceivesIPFromDHCP();
};

void RouterDHCPIntegrationTests::testRouterReceivesIPFromDHCP() {
    // Setup
    auto testPort = new Port();
    DHCPServer dhcpServer(1, testPort);

    // Use singleton for EventsCoordinator
    EventsCoordinator* coordinator = EventsCoordinator::instance();
    coordinator->start();

    auto router = QSharedPointer<Router>::create(1, "192.168.1.1", 6, nullptr);
    router->startRouter();
    coordinator->addRouter(router);

    // Connect DHCP server to router
    connect(router.data(), &Router::sendDHCPRequest, &dhcpServer, &DHCPServer::receivePacket);
    connect(&dhcpServer, &DHCPServer::broadcastPacket, router.data(), &Router::receiveIPFromDHCP);

    // Start coordinator clock
    coordinator->startClock(std::chrono::milliseconds(1000));

    QSignalSpy spy(router.data(), &Router::receiveIPFromDHCP);
    QVERIFY(spy.wait(3000)); // Wait for IP assignment

    // Verify IP was assigned
    auto packet = qvariant_cast<PacketPtr_t>(spy.takeFirst().at(0));
    QVERIFY(packet->getPayload().contains("192.168.100.1"));

    // Stop coordinator
    coordinator->stopClock();
    EventsCoordinator::release();
}

QTEST_MAIN(RouterDHCPIntegrationTests)
#include "RouterDHCPIntegrationTests.moc"
