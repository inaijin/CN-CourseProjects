#include <QtTest/QtTest>
#include "../Packet/Packet.h"
#include "../Port/Port.h"
#include "../DHCPServer/DHCPServer.h"

class DHCPServerTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testDHCPServerAssignsIP();
    void testDHCPServerReclaimsExpiredLeases();
};

void DHCPServerTests::testDHCPServerAssignsIP() {
    auto testPort = new Port();
    DHCPServer dhcpServer(1, testPort);

    auto requestPacket = QSharedPointer<Packet>::create(PacketType::Control, "DHCP_REQUEST:123");

    testPort->receivePacket(requestPacket);

    QSignalSpy spy(&dhcpServer, &DHCPServer::broadcastPacket);
    QVERIFY(spy.count() == 1);

    auto offerPacket = qvariant_cast<PacketPtr_t>(spy.takeFirst().at(0));
    QVERIFY(offerPacket->getPayload().contains("192.168.100.1"));
    QVERIFY(offerPacket->getPayload().contains("123"));
}

void DHCPServerTests::testDHCPServerReclaimsExpiredLeases() {

    auto testPort = new Port();
    DHCPServer dhcpServer(2, testPort);

    auto requestPacket1 = QSharedPointer<Packet>::create(PacketType::Control, "DHCP_REQUEST:101");
    auto requestPacket2 = QSharedPointer<Packet>::create(PacketType::Control, "DHCP_REQUEST:102");
    testPort->receivePacket(requestPacket1);
    testPort->receivePacket(requestPacket2);

    dhcpServer.tick(310);

    auto requestPacket3 = QSharedPointer<Packet>::create(PacketType::Control, "DHCP_REQUEST:103");
    testPort->receivePacket(requestPacket3);

    QSignalSpy spy(&dhcpServer, &DHCPServer::broadcastPacket);
    QVERIFY(spy.count() == 1);

    auto offerPacket = qvariant_cast<PacketPtr_t>(spy.takeFirst().at(0));
    QVERIFY(offerPacket->getPayload().contains("192.168.200.1"));
}

QTEST_MAIN(DHCPServerTests)
#include "DHCPServerTests.moc"
