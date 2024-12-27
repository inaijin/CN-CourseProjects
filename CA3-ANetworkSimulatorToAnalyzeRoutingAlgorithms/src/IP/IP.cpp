#include "IP.h"

IP::IP(const QSharedPointer<AbstractIPHeader> &header, QObject *parent) :
    QObject(parent),
    m_header(header)
{
}

IP::IP(const QString &ip, QObject *parent) :
    QObject(parent),
    m_ip(ip)
{
}

IP::~IP() {}

QSharedPointer<AbstractIPHeader> IP::getHeader() const
{
    return m_header;
}
