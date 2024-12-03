#include "IP.h"

AbstractIP::AbstractIP(QObject *parent) :
    QObject(parent)
{
}

// IPv4 Implementation
IP<UT::IPVersion::IPv4>::IP(QObject *parent) :
    AbstractIP(parent),
    m_versionHeaderLength(0),
    m_typeOfService(0),
    m_totalLength(0),
    m_identification(0),
    m_flagsFragmentOffset(0),
    m_ttl(0),
    m_protocol(0),
    m_headerChecksum(0),
    m_ipValue(0)
{
    m_sourceIp = nullptr;
    m_destIp = nullptr;
}

IP<UT::IPVersion::IPv4>::IP(const QString &ipString, QObject *parent) :
    IP(parent)
{
    fromString(ipString);
}

IP<UT::IPVersion::IPv4>::IP(uint32_t ipValue, QObject *parent) :
    IP(parent)
{
    fromValue(ipValue);
}

IP<UT::IPVersion::IPv4>::~IP()
{
}

QString IP<UT::IPVersion::IPv4>::toString() const
{
    return QString("%1.%2.%3.%4")
    .arg((m_ipValue >> 24) & 0xFF)
      .arg((m_ipValue >> 16) & 0xFF)
      .arg((m_ipValue >> 8) & 0xFF)
      .arg(m_ipValue & 0xFF);
}

uint32_t IP<UT::IPVersion::IPv4>::toValue() const
{
    return m_ipValue;
}

void IP<UT::IPVersion::IPv4>::fromString(const QString &ipString) {
    QHostAddress address(ipString);
    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        m_ipValue = address.toIPv4Address();
    } else {
        throw std::invalid_argument("Invalid IPv4 address: " + ipString.toStdString());
    }
}

void IP<UT::IPVersion::IPv4>::fromValue(uint32_t ipValue)
{
    m_ipValue = ipValue;
}

// IPv6 Implementation
IP<UT::IPVersion::IPv6>::IP(QObject *parent) :
    AbstractIP(parent),
    m_versionTrafficClassFlowLabel(0),
    m_payloadLength(0),
    m_nextHeader(0),
    m_hopLimit(0)
{
    m_sourceIp = nullptr;
    m_destIp = nullptr;
    memset(&m_ipValue, 0, sizeof(Q_IPV6ADDR));
}

IP<UT::IPVersion::IPv6>::IP(const QString &ipString, QObject *parent) :
    IP(parent)
{
    fromString(ipString);
}

IP<UT::IPVersion::IPv6>::IP(const Q_IPV6ADDR &ipValue, QObject *parent) :
    IP(parent)
{
    fromValue(ipValue);
}

IP<UT::IPVersion::IPv6>::~IP()
{
}

QString IP<UT::IPVersion::IPv6>::toString() const
{
    QHostAddress address(m_ipValue);
    return address.toString();
}

Q_IPV6ADDR IP<UT::IPVersion::IPv6>::toValue() const
{
    return m_ipValue;
}

void IP<UT::IPVersion::IPv6>::fromString(const QString &ipString) {
    QHostAddress address(ipString);
    if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        m_ipValue = address.toIPv6Address();
    } else {
        throw std::invalid_argument("Invalid IPv6 address: " + ipString.toStdString());
    }
}

void IP<UT::IPVersion::IPv6>::fromValue(const Q_IPV6ADDR &ipValue)
{
    m_ipValue = ipValue;
}

bool IP<UT::IPVersion::IPv6>::operator==(const IP<UT::IPVersion::IPv6> &other) const
{
    return memcmp(&m_ipValue, &other.m_ipValue, sizeof(Q_IPV6ADDR)) == 0;
}
