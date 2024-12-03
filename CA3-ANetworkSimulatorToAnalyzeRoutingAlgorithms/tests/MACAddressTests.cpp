#include <QtTest/QtTest>
#include <QSet>
#include "../src/MACAddress/MACAddress.h"
#include "../src/MACAddress/MACAddressGenerator.h"

class MACAddressTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testMACAddressDefaultConstructor();
    void testMACAddressParameterizedConstructor();
    void testMACAddressInvalidInput();

    void testMACAddressGeneration();
    void testMACAddressUniqueness();
};

void MACAddressTests::testMACAddressDefaultConstructor() {
    MACAddress mac;
    QCOMPARE(mac.toString(), QString("00:00:00:00:00:00"));
}

void MACAddressTests::testMACAddressParameterizedConstructor() {
    MACAddress mac("12:34:56:78:9A:BC");
    QCOMPARE(mac.toString(), QString("12:34:56:78:9A:BC"));
}

void MACAddressTests::testMACAddressInvalidInput() {
    try {
        MACAddress mac("INVALID:ADDRESS");
        QFAIL("Expected exception not thrown");
    } catch (const std::invalid_argument &e) {
        QVERIFY(true);
    }
}

void MACAddressTests::testMACAddressGeneration() {
    MACAddress mac = MACAddressGenerator::generate();
    QVERIFY(MACAddress::isValid(mac.toString()));
}

void MACAddressTests::testMACAddressUniqueness() {
    QSet<QString> generatedAddresses;
    for (int i = 0; i < 1000; ++i) {
        MACAddress mac = MACAddressGenerator::generate();
        QVERIFY(!generatedAddresses.contains(mac.toString()));
        generatedAddresses.insert(mac.toString());
    }
}

QTEST_MAIN(MACAddressTests)
#include "MACAddressTests.moc"
