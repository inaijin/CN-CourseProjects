#include "Packet.h"

Packet::Packet(PacketType type, const QString &payload) :
    m_type(type),
    m_payload(payload),
    m_waitCycles(0),
    m_queueWaitCycles(0),
    m_sequenceNumber(0),
    m_isDropped(false)
{
}

void Packet::setPayload(const QString &payload) {
    m_payload = payload;
}

QString Packet::getPayload() const {
    return m_payload;
}

void Packet::addToPath(const QString &routerIP) {
    m_path.append(routerIP);
}

QVector<QString> Packet::getPath() const {
    return m_path;
}

void Packet::incrementWaitCycles() {
    m_waitCycles++;
}

int Packet::getWaitCycles() const {
    return m_waitCycles;
}

void Packet::incrementQueueWaitCycles() {
    m_queueWaitCycles++;
}

int Packet::getQueueWaitCycles() const {
    return m_queueWaitCycles;
}

void Packet::setSequenceNumber(int sequenceNumber) {
    m_sequenceNumber = sequenceNumber;
}

int Packet::getSequenceNumber() const {
    return m_sequenceNumber;
}

void Packet::markDropped(bool isDropped) {
    m_isDropped = isDropped;
}

bool Packet::isDropped() const {
    return m_isDropped;
}

PacketType Packet::getType() const {
    return m_type;
}
