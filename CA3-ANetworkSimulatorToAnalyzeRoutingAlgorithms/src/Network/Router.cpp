#include "Router.h"

Router::Router(int id, const QString &ipAddress, int portCount, QObject *parent)
    : Node(id, ipAddress, parent), m_portCount(portCount)
{
    initializePorts();
}

Router::~Router()
{
    // Cleanup if necessary
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
    for (const auto &port : m_ports)
    {
        if (!port->isConnected())
        {
            return port;
        }
    }
    return nullptr;
}

std::vector<PortPtr_t> Router::getPorts()
{
    return m_ports;
}

void Router::run()
{
    // Thread execution starts here for the Router
    // For Phase 1, routers remain inactive
    // Implement the router's main loop in later phases
}
