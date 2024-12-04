#include "PC.h"

PC::PC(int id, const QString &ipAddress, QObject *parent)
    : QObject(parent), m_id(id), m_ipAddress(ipAddress)
{
    m_port = PortPtr_t::create(this);
    m_port->setPortNumber(1); // PCs typically have one port and in our project that is true also
    m_port->setRouterIP(m_ipAddress);
}

PC::~PC()
{
    // Cleanup if necessary
}

int PC::getId() const
{
    return m_id;
}

QString PC::getIPAddress() const
{
    return m_ipAddress;
}

PortPtr_t PC::getPort()
{
    return m_port;
}
