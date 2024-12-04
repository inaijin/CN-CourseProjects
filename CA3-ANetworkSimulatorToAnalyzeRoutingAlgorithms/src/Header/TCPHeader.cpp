#include "TCPHeader.h"

TCPHeader::TCPHeader(uint16_t sourcePort,
                     uint16_t destPort,
                     uint32_t sequenceNumber,
                     uint32_t acknowledgmentNumber,
                     uint8_t dataOffset,
                     uint8_t flags,
                     uint16_t windowSize,
                     uint16_t checksum,
                     uint16_t urgentPointer)
    : m_sourcePort(sourcePort),
    m_destPort(destPort),
    m_sequenceNumber(sequenceNumber),
    m_acknowledgmentNumber(acknowledgmentNumber),
    m_dataOffset(dataOffset),
    m_flags(flags),
    m_windowSize(windowSize),
    m_checksum(checksum),
    m_urgentPointer(urgentPointer)
{
}

void TCPHeader::setSourcePort(uint16_t sourcePort) {
    m_sourcePort = sourcePort;
}

uint16_t TCPHeader::getSourcePort() const {
    return m_sourcePort;
}

void TCPHeader::setDestPort(uint16_t destPort) {
    m_destPort = destPort;
}

uint16_t TCPHeader::getDestPort() const {
    return m_destPort;
}

void TCPHeader::setSequenceNumber(uint32_t sequenceNumber) {
    m_sequenceNumber = sequenceNumber;
}

uint32_t TCPHeader::getSequenceNumber() const {
    return m_sequenceNumber;
}

void TCPHeader::setAcknowledgmentNumber(uint32_t acknowledgmentNumber) {
    m_acknowledgmentNumber = acknowledgmentNumber;
}

uint32_t TCPHeader::getAcknowledgmentNumber() const {
    return m_acknowledgmentNumber;
}

void TCPHeader::setDataOffset(uint8_t dataOffset) {
    m_dataOffset = dataOffset;
}

uint8_t TCPHeader::getDataOffset() const {
    return m_dataOffset;
}

void TCPHeader::setFlags(uint8_t flags) {
    m_flags = flags;
}

uint8_t TCPHeader::getFlags() const {
    return m_flags;
}

void TCPHeader::setWindowSize(uint16_t windowSize) {
    m_windowSize = windowSize;
}

uint16_t TCPHeader::getWindowSize() const {
    return m_windowSize;
}

void TCPHeader::setChecksum(uint16_t checksum) {
    m_checksum = checksum;
}

uint16_t TCPHeader::getChecksum() const {
    return m_checksum;
}

void TCPHeader::setUrgentPointer(uint16_t urgentPointer) {
    m_urgentPointer = urgentPointer;
}

uint16_t TCPHeader::getUrgentPointer() const {
    return m_urgentPointer;
}

QString TCPHeader::toString() const {
    return QString("Source Port: %1, Destination Port: %2, Sequence Number: %3, Acknowledgment Number: %4, Data Offset: %5, Flags: %6, Window Size: %7, Checksum: %8, Urgent Pointer: %9")
    .arg(m_sourcePort)
      .arg(m_destPort)
      .arg(m_sequenceNumber)
      .arg(m_acknowledgmentNumber)
      .arg(m_dataOffset)
      .arg(m_flags)
      .arg(m_windowSize)
      .arg(m_checksum)
      .arg(m_urgentPointer);
}
