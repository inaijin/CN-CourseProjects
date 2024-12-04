#include "Port.h"
#include <QDebug>

Port::Port(QObject *parent) :
    QObject {parent},
    m_number(0),
    m_numberOfPacketsSent(0),
    m_routerIP(""),
    m_isConnected(false)
{}

Port::~Port() {}

void
Port::setPortNumber(uint8_t number)
{
    m_number = number;
}

uint8_t
Port::getPortNumber() const
{
    return m_number;
}

void
Port::setRouterIP(const QString &ip)
{
    m_routerIP = ip;
}

QString
Port::getRouterIP() const
{
    return m_routerIP;
}

bool
Port::isConnected() const
{
    return m_isConnected;
}

void
Port::setConnected(bool connected)
{
    m_isConnected = connected;
}

void
Port::sendPacket(const PacketPtr_t &data)
{
    ++m_numberOfPacketsSent;
    emit packetSent(data);
    qDebug() << "Port" << m_number << "on Router" << m_routerIP << "sent a packet.";
}

void
Port::receivePacket(const PacketPtr_t &data)
{
    emit packetReceived(data);
    qDebug() << "Port" << m_number << "on Router" << m_routerIP << "received a packet.";
}
