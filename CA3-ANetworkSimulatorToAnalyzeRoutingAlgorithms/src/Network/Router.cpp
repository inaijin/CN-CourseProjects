#include "Router.h"
#include <QDebug>

Router::Router(int id, const QString &ipAddress, int portCount, QObject *parent)
    : Node(id, ipAddress, NodeType::Router, parent), m_portCount(portCount)
{
    if (m_portCount <= 0)
    {
        m_portCount = 6;
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

void Router::run()
{
    qDebug() << "Router" << m_id << "is in idle mode.";
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

void Router::forwardPacket(const PacketPtr_t &packet)
{
    qDebug() << "Router" << m_id << "is forwarding a packet.";
    processPacket(packet);
}

void Router::processPacket(const PacketPtr_t &packet)
{
    qDebug() << "Router" << m_id << "is processing a packet.";
    packet->incrementWaitCycles(); // Dummy logic implement later !!!
}

void Router::logPortStatuses() const
{
    for (const auto &port : m_ports)
    {
        qDebug() << "Port" << port->getPortNumber() << (port->isConnected() ? "Connected" : "Available");
    }
}
