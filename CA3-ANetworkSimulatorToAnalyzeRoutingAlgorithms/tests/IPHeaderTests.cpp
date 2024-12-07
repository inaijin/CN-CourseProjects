#include <QtTest/QtTest>
#include "../src/IP/IPHeader.h"

class IPHeaderTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    // IPv4Header Tests
    void testIPv4DefaultConstructor();
    void testIPv4SerializationDeserialization();
    void testIPv4SetAndGetAddress();

    // IPv6Header Tests
    void testIPv6DefaultConstructor();
    void testIPv6SerializationDeserialization();
    void testIPv6SetAndGetAddress();
};

// IPv4Header Tests
void IPHeaderTests::testIPv4DefaultConstructor() {
    IPv4Header ipv4;
    QCOMPARE(ipv4.getSourceAddress(), QString(""));
    QCOMPARE(ipv4.getDestinationAddress(), QString(""));
}

void IPHeaderTests::testIPv4SerializationDeserialization() {
    IPv4Header ipv4;
    ipv4.setSourceAddress("192.168.1.1");
    ipv4.setDestinationAddress("192.168.1.2");

    QByteArray serializedData = ipv4.toBytes();

    IPv4Header deserializedIPv4;
    deserializedIPv4.fromBytes(serializedData);

    QCOMPARE(deserializedIPv4.getSourceAddress(), QString("192.168.1.1"));
    QCOMPARE(deserializedIPv4.getDestinationAddress(), QString("192.168.1.2"));
}

void IPHeaderTests::testIPv4SetAndGetAddress() {
    IPv4Header ipv4;
    ipv4.setSourceAddress("10.0.0.1");
    ipv4.setDestinationAddress("10.0.0.2");

    QCOMPARE(ipv4.getSourceAddress(), QString("10.0.0.1"));
    QCOMPARE(ipv4.getDestinationAddress(), QString("10.0.0.2"));
}

// IPv6Header Tests
void IPHeaderTests::testIPv6DefaultConstructor() {
    IPv6Header ipv6;
    QCOMPARE(ipv6.getSourceAddress(), QString(""));
    QCOMPARE(ipv6.getDestinationAddress(), QString(""));
}

void IPHeaderTests::testIPv6SerializationDeserialization() {
    IPv6Header ipv6;
    ipv6.setSourceAddress("2001:db8::1");
    ipv6.setDestinationAddress("2001:db8::2");

    QByteArray serializedData = ipv6.toBytes();

    IPv6Header deserializedIPv6;
    deserializedIPv6.fromBytes(serializedData);

    QCOMPARE(deserializedIPv6.getSourceAddress(), QString("2001:db8::1"));
    QCOMPARE(deserializedIPv6.getDestinationAddress(), QString("2001:db8::2"));
}

void IPHeaderTests::testIPv6SetAndGetAddress() {
    IPv6Header ipv6;
    ipv6.setSourceAddress("fe80::1");
    ipv6.setDestinationAddress("fe80::2");

    QCOMPARE(ipv6.getSourceAddress(), QString("fe80::1"));
    QCOMPARE(ipv6.getDestinationAddress(), QString("fe80::2"));
}

// QTEST_MAIN(IPHeaderTests)
#include "IPHeaderTests.moc"
