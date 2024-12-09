#include <QtTest/QtTest>
#include "../src/Header/TCPHeader.h"

class TCPHeaderTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testDefaultConstructor();
    void testParameterizedConstructor();
    void testFieldManagement();
};

void TCPHeaderTests::testDefaultConstructor() {
    TCPHeader header;

    QCOMPARE(header.getSourcePort(), static_cast<uint16_t>(0));
    QCOMPARE(header.getDestPort(), static_cast<uint16_t>(0));
    QCOMPARE(header.getSequenceNumber(), static_cast<uint32_t>(0));
    QCOMPARE(header.getAcknowledgmentNumber(), static_cast<uint32_t>(0));
    QCOMPARE(header.getDataOffset(), static_cast<uint8_t>(0));
    QCOMPARE(header.getFlags(), static_cast<uint8_t>(0));
    QCOMPARE(header.getWindowSize(), static_cast<uint16_t>(0));
    QCOMPARE(header.getChecksum(), static_cast<uint16_t>(0));
    QCOMPARE(header.getUrgentPointer(), static_cast<uint16_t>(0));
}

void TCPHeaderTests::testParameterizedConstructor() {
    TCPHeader header(80, 443, 12345, 67890, 5, 0b101010, 1024, 0xFFFF, 123);

    QCOMPARE(header.getSourcePort(), static_cast<uint16_t>(80));
    QCOMPARE(header.getDestPort(), static_cast<uint16_t>(443));
    QCOMPARE(header.getSequenceNumber(), static_cast<uint32_t>(12345));
    QCOMPARE(header.getAcknowledgmentNumber(), static_cast<uint32_t>(67890));
    QCOMPARE(header.getDataOffset(), static_cast<uint8_t>(5));
    QCOMPARE(header.getFlags(), static_cast<uint8_t>(0b101010));
    QCOMPARE(header.getWindowSize(), static_cast<uint16_t>(1024));
    QCOMPARE(header.getChecksum(), static_cast<uint16_t>(0xFFFF));
    QCOMPARE(header.getUrgentPointer(), static_cast<uint16_t>(123));
}

void TCPHeaderTests::testFieldManagement() {
    TCPHeader header;

    header.setSourcePort(8080);
    QCOMPARE(header.getSourcePort(), static_cast<uint16_t>(8080));

    header.setDestPort(8443);
    QCOMPARE(header.getDestPort(), static_cast<uint16_t>(8443));

    header.setSequenceNumber(54321);
    QCOMPARE(header.getSequenceNumber(), static_cast<uint32_t>(54321));

    header.setAcknowledgmentNumber(98765);
    QCOMPARE(header.getAcknowledgmentNumber(), static_cast<uint32_t>(98765));

    header.setDataOffset(10);
    QCOMPARE(header.getDataOffset(), static_cast<uint8_t>(10));

    header.setFlags(0b111000);
    QCOMPARE(header.getFlags(), static_cast<uint8_t>(0b111000));

    header.setWindowSize(2048);
    QCOMPARE(header.getWindowSize(), static_cast<uint16_t>(2048));

    header.setChecksum(0xABCD);
    QCOMPARE(header.getChecksum(), static_cast<uint16_t>(0xABCD));

    header.setUrgentPointer(456);
    QCOMPARE(header.getUrgentPointer(), static_cast<uint16_t>(456));
}

QTEST_MAIN(TCPHeaderTests)
#include "TCPHeaderTests.moc"
