#include "IPHeader.h"
#include <QDataStream>

AbstractIPHeader::AbstractIPHeader(QObject *parent) : QObject(parent) {}
AbstractIPHeader::~AbstractIPHeader() {}

// IPv4Header Implementation
IPv4Header::IPv4Header(QObject *parent) :
    AbstractIPHeader(parent),
    m_versionHeaderLength(0x45),
    m_typeOfService(0),
    m_totalLength(0),
    m_identification(0),
    m_flagsFragmentOffset(0),
    m_ttl(64),
    m_protocol(0),
    m_headerChecksum(0)
{
}

IPv4Header::~IPv4Header() {}

QByteArray IPv4Header::toBytes() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_versionHeaderLength
           << m_typeOfService
           << m_totalLength
           << m_identification
           << m_flagsFragmentOffset
           << m_ttl
           << m_protocol
           << m_headerChecksum
           << m_sourceAddress.toUtf8()
           << m_destinationAddress.toUtf8();
    return data;
}

void IPv4Header::fromBytes(const QByteArray &data)
{
    QDataStream stream(data);
    stream >> m_versionHeaderLength
      >> m_typeOfService
      >> m_totalLength
      >> m_identification
      >> m_flagsFragmentOffset
      >> m_ttl
      >> m_protocol
      >> m_headerChecksum;
    QByteArray src, dest;
    stream >> src >> dest;
    m_sourceAddress = QString::fromUtf8(src);
    m_destinationAddress = QString::fromUtf8(dest);
}

void IPv4Header::setSourceAddress(const QString &address)
{
    m_sourceAddress = address;
}

void IPv4Header::setDestinationAddress(const QString &address)
{
    m_destinationAddress = address;
}

QString IPv4Header::getSourceAddress() const
{
    return m_sourceAddress;
}

QString IPv4Header::getDestinationAddress() const
{
    return m_destinationAddress;
}

// IPv6Header Implementation
IPv6Header::IPv6Header(QObject *parent) :
    AbstractIPHeader(parent),
    m_versionTrafficClassFlowLabel(0),
    m_payloadLength(0),
    m_nextHeader(0),
    m_hopLimit(64)
{
}

IPv6Header::~IPv6Header() {}

QByteArray IPv6Header::toBytes() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_versionTrafficClassFlowLabel
           << m_payloadLength
           << m_nextHeader
           << m_hopLimit
           << m_sourceAddress.toUtf8()
           << m_destinationAddress.toUtf8();
    return data;
}

void IPv6Header::fromBytes(const QByteArray &data)
{
    QDataStream stream(data);
    stream >> m_versionTrafficClassFlowLabel
      >> m_payloadLength
      >> m_nextHeader
      >> m_hopLimit;
    QByteArray src, dest;
    stream >> src >> dest;
    m_sourceAddress = QString::fromUtf8(src);
    m_destinationAddress = QString::fromUtf8(dest);
}

void IPv6Header::setSourceAddress(const QString &address)
{
    m_sourceAddress = address;
}

void IPv6Header::setDestinationAddress(const QString &address)
{
    m_destinationAddress = address;
}

QString IPv6Header::getSourceAddress() const
{
    return m_sourceAddress;
}

QString IPv6Header::getDestinationAddress() const
{
    return m_destinationAddress;
}
