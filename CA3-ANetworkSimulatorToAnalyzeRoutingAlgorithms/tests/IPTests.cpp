#include <QtTest/QtTest>
#include "../src/IP/IP.h"

class IPTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    // IPv4 Tests
    void testIPv4DefaultConstructor();
    void testIPv4ParameterizedConstructor();
    void testIPv4InvalidInput();

    // IPv6 Tests
    void testIPv6DefaultConstructor();
    void testIPv6ParameterizedConstructor();
    void testIPv6InvalidInput();
};

void IPTests::testIPv4DefaultConstructor() {
    IP<UT::IPVersion::IPv4> ipv4;
    QCOMPARE(ipv4.toString(), QString("0.0.0.0"));
}

void IPTests::testIPv4ParameterizedConstructor() {
    IP<UT::IPVersion::IPv4> ipv4("192.168.1.1");
    QCOMPARE(ipv4.toString(), QString("192.168.1.1"));
}

void IPTests::testIPv4InvalidInput() {
    try {
        IP<UT::IPVersion::IPv4> ipv4("INVALID_IP");
        QFAIL("Expected exception not thrown");
    } catch (const std::invalid_argument &e) {
        QVERIFY(true);
    }
}

void IPTests::testIPv6DefaultConstructor() {
    IP<UT::IPVersion::IPv6> ipv6;
    QCOMPARE(ipv6.toString(), QString("::"));
}

void IPTests::testIPv6ParameterizedConstructor() {
    IP<UT::IPVersion::IPv6> ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
    QCOMPARE(ipv6.toString(), QString("2001:db8:85a3::8a2e:370:7334"));
}

void IPTests::testIPv6InvalidInput() {
    try {
        IP<UT::IPVersion::IPv6> ipv6("INVALID_IP");
        QFAIL("Expected exception not thrown");
    } catch (const std::invalid_argument &e) {
        QVERIFY(true);
    }
}

// QTEST_MAIN(IPTests)
#include "IPTests.moc"
