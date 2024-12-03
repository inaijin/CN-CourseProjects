#include "IP.h"

/**
 * Implementation of AbstractIP
 */
AbstractIP::AbstractIP(QObject *parent) :
    QObject(parent),
    m_ipValue(std::numeric_limits<uint64_t>::max())
{
}

/**
 * Implementation of IPv4
 */
IP<UT::IPVersion::IPv4>::IP(QObject *parent) :
    AbstractIP(parent)
{
}

IP<UT::IPVersion::IPv4>::IP(const QString &ipString, QObject *parent) :
    AbstractIP(parent)
{
    fromString(ipString);
}

IP<UT::IPVersion::IPv4>::IP(uint64_t ipValue, QObject *parent) :
    AbstractIP(parent)
{
    fromValue(ipValue);
}

IP<UT::IPVersion::IPv4>::~IP()
{
}

QString IP<UT::IPVersion::IPv4>::toString() const
{
    return toStringImpl();
}

uint64_t IP<UT::IPVersion::IPv4>::toValue() const
{
    return m_ipValue;
}

void IP<UT::IPVersion::IPv4>::fromString(const QString &ipString)
{
    fromStringImpl(ipString);
}

void IP<UT::IPVersion::IPv4>::fromValue(uint64_t ipValue)
{
    m_ipValue = ipValue;
}

QString IP<UT::IPVersion::IPv4>::toStringImpl() const
{
    // Convert IPv4 value to dotted string format
    return QString("%1.%2.%3.%4")
      .arg((m_ipValue >> 24) & 0xFF)
      .arg((m_ipValue >> 16) & 0xFF)
      .arg((m_ipValue >> 8) & 0xFF)
      .arg(m_ipValue & 0xFF);
}

void IP<UT::IPVersion::IPv4>::fromStringImpl(const QString &ipString)
{
    QStringList octets = ipString.split(".");
    if (octets.size() == 4)
    {
        m_ipValue = (octets[0].toUInt() << 24) |
                    (octets[1].toUInt() << 16) |
                    (octets[2].toUInt() << 8) |
                    octets[3].toUInt();
    }
}

/**
 * Implementation of IPv6
 */
IP<UT::IPVersion::IPv6>::IP(QObject *parent) :
    AbstractIP(parent)
{
}

IP<UT::IPVersion::IPv6>::IP(const QString &ipString, QObject *parent) :
    AbstractIP(parent)
{
    fromString(ipString);
}

IP<UT::IPVersion::IPv6>::IP(uint64_t ipValue, QObject *parent) :
    AbstractIP(parent)
{
    fromValue(ipValue);
}

IP<UT::IPVersion::IPv6>::~IP()
{
}

QString IP<UT::IPVersion::IPv6>::toString() const
{
    return toStringImpl();
}

uint64_t IP<UT::IPVersion::IPv6>::toValue() const
{
    return m_ipValue;
}

void IP<UT::IPVersion::IPv6>::fromString(const QString &ipString)
{
    fromStringImpl(ipString);
}

void IP<UT::IPVersion::IPv6>::fromValue(uint64_t ipValue)
{
    m_ipValue = ipValue;
}

QString IP<UT::IPVersion::IPv6>::toStringImpl() const
{
    // Convert IPv6 value to string (placeholder implementation)
    return QString("IPv6 representation");
}

void IP<UT::IPVersion::IPv6>::fromStringImpl(const QString &ipString)
{
    // Convert string to IPv6 value (placeholder implementation)
    m_ipValue = 0; // Replace with actual conversion logic
}
