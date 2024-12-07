#ifndef PORT_H
#define PORT_H

#include "../Packet/Packet.h"

#include <QObject>
#include <QMutex>

class Port : public QObject
{
    Q_OBJECT

public:
    explicit Port(QObject *parent = nullptr);
    ~Port() override;

    void setPortNumber(uint8_t number);
    uint8_t getPortNumber() const;

    void setRouterIP(const QString &ip);
    QString getRouterIP() const;

    bool isConnected() const;
    void setConnected(bool connected);

    uint64_t getNumberOfPacketsSent() const;
    uint64_t getNumberOfPacketsReceived() const;

Q_SIGNALS:
    void packetSent(const PacketPtr_t &data);
    void packetReceived(const PacketPtr_t &data);

public Q_SLOTS:
    void sendPacket(const PacketPtr_t &data);
    void receivePacket(const PacketPtr_t &data);

private:
    uint8_t  m_number;
    uint64_t m_numberOfPacketsSent;
    uint64_t m_numberOfPacketsReceived;
    QString  m_routerIP;
    bool     m_isConnected;

    mutable QMutex m_mutex;
};

typedef QSharedPointer<Port> PortPtr_t;

#endif    // PORT_H
