#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <cstdint>
#include <QString>

class TCPHeader
{
public:
    explicit TCPHeader(uint16_t sourcePort = 0,
                       uint16_t destPort = 0,
                       uint32_t sequenceNumber = 0,
                       uint32_t acknowledgmentNumber = 0,
                       uint8_t dataOffset = 0,
                       uint8_t flags = 0,
                       uint16_t windowSize = 0,
                       uint16_t checksum = 0,
                       uint16_t urgentPointer = 0);

    void setSourcePort(uint16_t sourcePort);
    uint16_t getSourcePort() const;

    void setDestPort(uint16_t destPort);
    uint16_t getDestPort() const;

    void setSequenceNumber(uint32_t sequenceNumber);
    uint32_t getSequenceNumber() const;

    void setAcknowledgmentNumber(uint32_t acknowledgmentNumber);
    uint32_t getAcknowledgmentNumber() const;

    void setDataOffset(uint8_t dataOffset);
    uint8_t getDataOffset() const;

    void setFlags(uint8_t flags);
    uint8_t getFlags() const;

    void setWindowSize(uint16_t windowSize);
    uint16_t getWindowSize() const;

    void setChecksum(uint16_t checksum);
    uint16_t getChecksum() const;

    void setUrgentPointer(uint16_t urgentPointer);
    uint16_t getUrgentPointer() const;

    QString toString() const;

private:
    uint16_t m_sourcePort;
    uint16_t m_destPort;
    uint32_t m_sequenceNumber;
    uint32_t m_acknowledgmentNumber;
    uint8_t m_dataOffset;
    uint8_t m_flags;  // Flags like URG, ACK, PSH, RST, SYN, FIN
    uint16_t m_windowSize;
    uint16_t m_checksum;
    uint16_t m_urgentPointer;
};

#endif // TCPHEADER_H
