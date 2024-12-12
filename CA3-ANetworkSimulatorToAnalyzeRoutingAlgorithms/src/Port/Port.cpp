#include "Port.h"
#include <QDebug>

Port::Port(QObject *parent) :
    QObject {parent},
    m_number(0),
    m_numberOfPacketsSent(0),
    m_numberOfPacketsReceived(0),
    m_routerIP(""),
    m_isConnected(false)
{}

Port::~Port() {}

void Port::setPortNumber(uint8_t number)
{
    QMutexLocker locker(&m_mutex);
    m_number = number;
}

uint8_t Port::getPortNumber() const
{
    QMutexLocker locker(&m_mutex);
    return m_number;
}

void Port::setRouterIP(const QString &ip)
{
    QMutexLocker locker(&m_mutex);
    m_routerIP = ip;
}

QString Port::getRouterIP() const
{
    QMutexLocker locker(&m_mutex);
    return m_routerIP;
}

bool Port::isConnected() const
{
    QMutexLocker locker(&m_mutex);
    return m_isConnected;
}

void Port::setConnected(bool connected)
{
    QMutexLocker locker(&m_mutex);
    m_isConnected = connected;
}

uint64_t Port::getNumberOfPacketsSent() const
{
    QMutexLocker locker(&m_mutex);
    return m_numberOfPacketsSent;
}

uint64_t Port::getNumberOfPacketsReceived() const
{
    QMutexLocker locker(&m_mutex);
    return m_numberOfPacketsReceived;
}

void Port::sendPacket(const PacketPtr_t &data) {
    QMutexLocker locker(&m_mutex);
    ++m_numberOfPacketsSent;
    emit packetSent(data);
    qDebug() << "Port" << m_number << "on Router" << m_routerIP << "sent packet with payload:" << data->getPayload();
}

void Port::receivePacket(const PacketPtr_t &data) {
    QMutexLocker locker(&m_mutex);
    ++m_numberOfPacketsReceived;
    emit packetReceived(data);
    qDebug() << "Port" << m_number << "on Router" << m_routerIP << "received packet with payload:" << data->getPayload();
}
