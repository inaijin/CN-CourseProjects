#include <QtTest/QtTest>
#include "../Network/PC.h"

class PCTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testPCInitialization();
};

void PCTests::testPCInitialization() {
    PC pc(1, "192.168.1.10");
    QCOMPARE(pc.getId(), 1);
    QCOMPARE(pc.getIPAddress(), QString("192.168.1.10"));
}

// QTEST_MAIN(PCTests)
#include "PCTests.moc"
