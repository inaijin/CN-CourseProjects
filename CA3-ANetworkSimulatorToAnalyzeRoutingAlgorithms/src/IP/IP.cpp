#include "IP.h"
#include <QRegularExpression>

IP::IP(const QSharedPointer<AbstractIPHeader> &header, QObject *parent) :
    QObject(parent),
    m_header(header)
{
    m_isIPv6 = false;
}

IP::IP(const QString &ip, QObject *parent) :
    QObject(parent),
    m_ip(ip)
{
    m_isIPv6 = false;
}

IP::~IP() {}

QSharedPointer<AbstractIPHeader> IP::getHeader() const
{
    return m_header;
}

bool isIPv4MappedIPv6(const QString &ipv6)
{
    QRegularExpression static regex("^::ffff:(\\d{1,3}\\.){3}\\d{1,3}$");
    return regex.match(ipv6).hasMatch();
}

QString extractIPv4FromIPv6(const QString &ipv6)
{
    return ipv6.section(":", -1);
}

bool IP::convertToIPv6()
{
    if (!m_header)
        return false;

    if (dynamic_cast<IPv6Header*>(m_header.data()))
        return false;

    IPv4Header* ipv4Header = dynamic_cast<IPv4Header*>(m_header.data());
    if (!ipv4Header)
        return false;

    QSharedPointer<IPv6Header> ipv6Header = QSharedPointer<IPv6Header>::create();

    QString srcIPv4 = ipv4Header->getSourceAddress();
    QString destIPv4 = ipv4Header->getDestinationAddress();

    QString srcIPv6 = QString("::ffff:%1").arg(srcIPv4);
    QString destIPv6 = QString("::ffff:%1").arg(destIPv4);

    ipv6Header->setSourceAddress(srcIPv6);
    ipv6Header->setDestinationAddress(destIPv6);

    m_header = ipv6Header;
    m_ip = destIPv6;

    return true;
}

bool IP::convertToIPv4()
{
    if (!m_header)
        return false;

    if (dynamic_cast<IPv4Header*>(m_header.data()))
        return false;

    IPv6Header* ipv6Header = dynamic_cast<IPv6Header*>(m_header.data());
    if (!ipv6Header)
        return false;

    QString srcIPv6 = ipv6Header->getSourceAddress();
    QString destIPv6 = ipv6Header->getDestinationAddress();

    if (!isIPv4MappedIPv6(srcIPv6) || !isIPv4MappedIPv6(destIPv6))
        return false;

    QString srcIPv4 = extractIPv4FromIPv6(srcIPv6);
    QString destIPv4 = extractIPv4FromIPv6(destIPv6);

    QSharedPointer<IPv4Header> ipv4Header = QSharedPointer<IPv4Header>::create();

    ipv4Header->setSourceAddress(srcIPv4);
    ipv4Header->setDestinationAddress(destIPv4);

    m_header = ipv4Header;
    m_ip = destIPv4;

    return true;
}
