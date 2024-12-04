#include "Router.h"

Router::Router(int id, const QString &ipAddress, QObject *parent)
    : QThread(parent), m_id(id), m_ipAddress(ipAddress), m_portCount(6) // Assuming 6 ports as default
{
    initializePorts();
}

Router::~Router()
{
    // Cleanup if necessary
}

int Router::getId() const
{
    return m_id;
}

QString Router::getIPAddress() const
{
    return m_ipAddress;
}

void Router::initializePorts()
{
    for (int i = 0; i < m_portCount; ++i)
    {
        auto port = PortPtr_t::create(this);
        port->setPortNumber(static_cast<uint8_t>(i + 1));
        port->setRouterIP(m_ipAddress);
        m_ports.push_back(port);
    }
}

PortPtr_t Router::getAvailablePort()
{
    // Return the first unconnected port
    for (const auto &port : m_ports)
    {
        if (!port->isConnected())
        {
            return port;
        }
    }
    return nullptr; // No available ports
}

void Router::run()
{
    // For Phase 1, routers remain inactive
}
