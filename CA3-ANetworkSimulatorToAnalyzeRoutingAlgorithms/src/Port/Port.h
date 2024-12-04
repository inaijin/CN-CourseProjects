#ifndef PORT_H
#define PORT_H

#include "../Packet/Packet.h"

#include <QObject>

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

Q_SIGNALS:
    void packetSent(const PacketPtr_t &data);
    void packetReceived(const PacketPtr_t &data);

public Q_SLOTS:
    void sendPacket(const PacketPtr_t &data);
    void receivePacket(const PacketPtr_t &data);

private:
    uint8_t  m_number = 0;
    uint64_t m_numberOfPacketsSent = 0;
    QString  m_routerIP;
    bool     m_isConnected = false;
};

typedef QSharedPointer<Port> PortPtr_t;

#endif    // PORT_H
