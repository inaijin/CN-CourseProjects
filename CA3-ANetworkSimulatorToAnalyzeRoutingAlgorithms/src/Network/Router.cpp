#include "Router.h"
#include <QDebug>

Router::Router(int id, const QString &ipAddress, int portCount, QObject *parent)
    : Node(id, ipAddress, parent), m_portCount(portCount)
{
    if (m_portCount <= 0)
    {
        m_portCount = 6; // Default to 6 ports if invalid value provided
    }

    initializePorts();
    qDebug() << "Router initialized: ID =" << m_id << ", IP =" << m_ipAddress << ", Ports =" << m_portCount;
}

Router::~Router()
{
    if (isRunning())
    {
        quit();
        wait();
    }

    qDebug() << "Router destroyed: ID =" << m_id;
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

void Router::logPortStatuses() const
{
    for (const auto &port : m_ports)
    {
        qDebug() << "Port" << port->getPortNumber() << (port->isConnected() ? "Connected" : "Available");
    }
}

void Router::run()
{
    qDebug() << "Router" << m_id << "is in idle mode.";
    // Implement the router's main loop in later phases
    exec();
}

void Router::startRouter()
{
    qDebug() << "Starting Router" << m_id;
    if (!isRunning())
    {
        start();
    }
}
