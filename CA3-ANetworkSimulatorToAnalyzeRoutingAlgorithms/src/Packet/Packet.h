#ifndef PACKET_H
#define PACKET_H

#include <QString>
#include <QVector>
#include <QSharedPointer>
#include "../Header/DataLinkHeader.h"
#include "../Header/TCPHeader.h"

enum class PacketType {
    Data,
    Control,
    RIPUpdate,
    OSPFHello,
    DHCPRequest,
    DHCPOffer,
    Custom
};

class Packet
{
public:
    explicit Packet(PacketType type = PacketType::Data, const QString &payload = "");

    void setPayload(const QString &payload);
    QString getPayload() const;

    void addToPath(const QString &routerIP);
    QVector<QString> getPath() const;

    void incrementWaitCycles();
    int getWaitCycles() const;

    void incrementQueueWaitCycles();
    int getQueueWaitCycles() const;

    void setSequenceNumber(int sequenceNumber);
    int getSequenceNumber() const;

    void markDropped(bool isDropped);
    bool isDropped() const;

    PacketType getType() const;

    // DataLinkHeader Methods
    void setDataLinkHeader(const DataLinkHeader &header);
    DataLinkHeader getDataLinkHeader() const;

    // TCPHeader Methods
    void setTCPHeader(const TCPHeader &header);
    TCPHeader getTCPHeader() const;

private:
    PacketType m_type;                  // Type of the packet (data, control, etc.)
    QString m_payload;                  // The data or information the packet carries
    QVector<QString> m_path;            // Path the packet has traversed (IP addresses)
    int m_waitCycles;                   // Total cycles the packet has waited
    int m_queueWaitCycles;              // Cycles spent in a queue
    int m_sequenceNumber;               // Sequence number for tracking
    bool m_isDropped;                   // Flag indicating if the packet was dropped

    DataLinkHeader m_dataLinkHeader;    // DataLink layer header
    TCPHeader m_tcpHeader;              // TCP layer header
};

typedef QSharedPointer<Packet> PacketPtr_t;

#endif // PACKET_H
