#include "Packet.h"

qint64 Packet::s_nextId = 0;

Packet::Packet(PacketType type, const QString &payload)
    : m_type(type),
    m_payload(payload),
    m_waitCycles(0),
    m_queueWaitCycles(0),
    m_sequenceNumber(0),
    m_isDropped(false),
    m_dataLinkHeader(),
    m_tcpHeader(),
    m_ttl(10), // Default TTL for all packets
    m_id(++s_nextId)
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

void Packet::setDataLinkHeader(const DataLinkHeader &header) {
    m_dataLinkHeader = header;
}

DataLinkHeader Packet::getDataLinkHeader() const {
    return m_dataLinkHeader;
}

void Packet::setTCPHeader(const TCPHeader &header) {
    m_tcpHeader = header;
}

TCPHeader Packet::getTCPHeader() const {
    return m_tcpHeader;
}

void Packet::setTTL(int ttl) {
    m_ttl = ttl;
}

int Packet::getTTL() const {
    return m_ttl;
}

void Packet::decrementTTL() {
    m_ttl--;
}

qint64 Packet::getId() const {
    return m_id;
}
