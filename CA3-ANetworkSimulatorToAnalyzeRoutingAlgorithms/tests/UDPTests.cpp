#include <QtTest/QtTest>
#include "../Port/Port.h"
#include "../PortBindingManager/PortBindingManager.h"
#include "../Packet/Packet.h"
#include "../BroadCast/UDP.h"
#include "../Network/Router.h"

class UDPTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testBroadcast();
};

void UDPTests::testBroadcast() {
    auto testRouter = QSharedPointer<Router>::create(1, "192.168.1.1", 4);
    testRouter->startRouter();

    QVector<QSharedPointer<Port>> connectedPorts;
    for (int i = 0; i < 4; ++i) {
        auto connectedPort = QSharedPointer<Port>::create();
        connectedPort->setPortNumber(i + 1);
        connectedPort->setConnected(true);
        connectedPorts.push_back(connectedPort);

        PortBindingManager bindingManager;
        bindingManager.bind(testRouter->getPorts()[i], connectedPort);
    }

    UDP udp;

    auto packet = QSharedPointer<Packet>::create(PacketType::Data, "TestPayload");

    udp.broadcastPacket(packet, testRouter);

    for (const auto &port : connectedPorts) {
        QCOMPARE(port->getNumberOfPacketsReceived(), static_cast<uint64_t>(1));
    }

    testRouter->quit();
    testRouter->wait();
}

// QTEST_MAIN(UDPTests)
#include "UDPTests.moc"
