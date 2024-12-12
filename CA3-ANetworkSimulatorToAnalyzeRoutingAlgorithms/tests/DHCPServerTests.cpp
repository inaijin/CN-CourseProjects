
#include <QtTest/QtTest>
#include "../Packet/Packet.h"
#include "../Port/Port.h"
#include "../Network/Router.h"
#include "../BroadCast/UDP.h"
#include "../DHCP/DHCPServer.h"

class DHCPServerTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testDHCPServerAssignsIP();
    void testDHCPServerReclaimsExpiredLeases();
};

void DHCPServerTests::testDHCPServerAssignsIP() {
    // Setup: Create a port and DHCP server for AS1
    auto testPort = new Port();
    DHCPServer dhcpServer(1, testPort);

    // Create a simulated DHCP_REQUEST packet
    auto requestPacket = QSharedPointer<Packet>::create(PacketType::Control, "DHCP_REQUEST:123");

    // Send the request packet to the DHCP server
    testPort->receivePacket(requestPacket);

    // Verify the DHCPServer emits a broadcast with the correct IP offer
    QSignalSpy spy(&dhcpServer, &DHCPServer::broadcastPacket);
    QVERIFY(spy.count() == 1);

    // Verify the content of the broadcast packet
    auto offerPacket = qvariant_cast<PacketPtr_t>(spy.takeFirst().at(0));
    QVERIFY(offerPacket->getPayload().contains("192.168.100.1"));
    QVERIFY(offerPacket->getPayload().contains("123"));
}

void DHCPServerTests::testDHCPServerReclaimsExpiredLeases() {
    // Setup: Create a port and DHCP server for AS2
    auto testPort = new Port();
    DHCPServer dhcpServer(2, testPort);

    // Simulate two DHCP_REQUEST packets
    auto requestPacket1 = QSharedPointer<Packet>::create(PacketType::Control, "DHCP_REQUEST:101");
    auto requestPacket2 = QSharedPointer<Packet>::create(PacketType::Control, "DHCP_REQUEST:102");
    testPort->receivePacket(requestPacket1);
    testPort->receivePacket(requestPacket2);

    // Fast-forward time to simulate lease expiration
    dhcpServer.tick(310); // Assuming LEASE_DURATION is 300

    // Verify the leases have been reclaimed
    // Since we don't have direct access to the internal lease list, rely on new requests
    auto requestPacket3 = QSharedPointer<Packet>::create(PacketType::Control, "DHCP_REQUEST:103");
    testPort->receivePacket(requestPacket3);

    QSignalSpy spy(&dhcpServer, &DHCPServer::broadcastPacket);
    QVERIFY(spy.count() == 1); // Only one offer since the previous leases expired

    auto offerPacket = qvariant_cast<PacketPtr_t>(spy.takeFirst().at(0));
    QVERIFY(offerPacket->getPayload().contains("192.168.200.1")); // First IP reused
}

QTEST_MAIN(DHCPServerTests)
#include "DHCPServerTests.moc"
