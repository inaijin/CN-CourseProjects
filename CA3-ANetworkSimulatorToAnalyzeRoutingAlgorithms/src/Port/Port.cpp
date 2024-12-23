#include "Port.h"
#include "../Network/PC.h"
#include <QDebug>

Port::Port(QObject *parent) :
    QObject {parent},
    m_number(0),
    m_numberOfPacketsSent(0),
    m_numberOfPacketsReceived(0),
    m_routerIP(""),
    m_isConnected(false),
    m_connectedPC(nullptr)
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
    {
        QMutexLocker locker(&m_mutex);
        ++m_numberOfPacketsSent;
    }
    emit packetSent(data);
    qDebug() << "Port::sendPacket() emitted packetSent.";
}

void Port::receivePacket(const PacketPtr_t &data) {
    {
        QMutexLocker locker(&m_mutex);
        ++m_numberOfPacketsReceived;
    }
    emit packetReceived(data);
    qDebug() << "Port::receivePacket() emitted packetReceived.";
}

void Port::setConnectedRouterId(int routerId) {
    QMutexLocker locker(&m_mutex);
    m_connectedRouterId = routerId;
}

int Port::getConnectedRouterId() const {
    QMutexLocker locker(&m_mutex);
    return m_connectedRouterId;
}

void Port::connectToPC(QSharedPointer<PC> pc)
{
    QMutexLocker locker(&m_mutex);
    if (m_connectedPC) {
        qWarning() << "Port" << m_number << "is already connected to PC with IP" << m_connectedPC->getIpAddress();
        return;
    }

    m_connectedPC = pc;
    m_isConnected = true; // Mark as connected

    qDebug() << "Port" << m_number << "connected to PC with IP" << pc->getIpAddress();

    // Connect signals: When the port receives a packet, send it to the PC
    connect(this, &Port::packetReceived, pc.data(), &PC::processPacket);
    // If PC needs to send packets through the port, connect PC's send signal to port's sendPacket slot
    connect(pc.data(), &PC::packetSent, this, &Port::sendPacket);
}

// New method to get connected PC
QSharedPointer<PC> Port::getConnectedPC() const
{
    QMutexLocker locker(&m_mutex);
    return m_connectedPC;
}
