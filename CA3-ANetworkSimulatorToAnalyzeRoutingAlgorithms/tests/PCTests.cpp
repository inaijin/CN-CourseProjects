#include <QtTest/QtTest>
#include "../src/Network/PC.h"

class PCTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testPCInitialization();
    void testPacketGeneration(); // Placeholder for future phases implementation
};

void PCTests::testPCInitialization()
{
    PC pc("192.168.1.100");
    QCOMPARE(pc.getId(), 1);
    QCOMPARE(pc.getIPAddress(), QString("192.168.1.100"));
    QVERIFY(pc.getPort() != nullptr);
}

void PCTests::testPacketGeneration()
{
    // Placeholder for packet generation test in future phases
    QVERIFY(true);
}

// QTEST_MAIN(PCTests)
#include "PCTests.moc"
