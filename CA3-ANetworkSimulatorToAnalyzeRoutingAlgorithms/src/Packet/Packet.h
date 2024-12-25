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
    OSPFLSA,
    DHCPRequest,
    DHCPOffer,
    Custom
};

class Packet
{
public:
    explicit Packet(PacketType type = PacketType::Data, const QString &payload = "");
    Packet(PacketType type, const QString &payload, int ttl); // New constructor

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

    // TTL methods
    void setTTL(int ttl);
    int getTTL() const;
    void decrementTTL();

    // DataLinkHeader Methods
    void setDataLinkHeader(const DataLinkHeader &header);
    DataLinkHeader getDataLinkHeader() const;

    // TCPHeader Methods
    void setTCPHeader(const TCPHeader &header);
    TCPHeader getTCPHeader() const;

    qint64 getId() const; // Unique packet identifier

private:
    static qint64 s_nextId;

    PacketType m_type;
    QString m_payload;
    QVector<QString> m_path;
    int m_waitCycles;
    int m_queueWaitCycles;
    int m_sequenceNumber;
    bool m_isDropped;
    DataLinkHeader m_dataLinkHeader;
    TCPHeader m_tcpHeader;
    int m_ttl;
    qint64 m_id;
};

typedef QSharedPointer<Packet> PacketPtr_t;

#endif // PACKET_H
