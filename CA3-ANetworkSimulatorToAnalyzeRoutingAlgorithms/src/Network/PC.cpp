#include "PC.h"
#include <QDebug>

PC::PC(int id, const QString &ipAddress, QObject *parent)
    : Node(id, ipAddress, parent)
{
    m_port = PortPtr_t::create(this);
    m_port->setPortNumber(1);
    m_port->setRouterIP(m_ipAddress);

    qDebug() << "PC initialized: ID =" << m_id << ", IP =" << m_ipAddress;
}

PC::~PC()
{
    qDebug() << "PC destroyed: ID =" << m_id;
}

PortPtr_t PC::getPort()
{
    return m_port;
}

void PC::run()
{
    qDebug() << "PC" << m_id << "is in idle mode.";
    // Implement PC's main loop in later phases
    exec();
}

void PC::startPC()
{
    qDebug() << "Starting PC" << m_id;
    if (!isRunning())
    {
        start();
    }
}
