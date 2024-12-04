#include "PC.h"

PC::PC(int id, const QString &ipAddress, QObject *parent)
    : Node(id, ipAddress, parent)
{
    m_port = PortPtr_t::create(this);
    m_port->setPortNumber(1);
    m_port->setRouterIP(m_ipAddress);
}

PC::~PC()
{
    // Cleanup if necessary
}

PortPtr_t PC::getPort()
{
    return m_port;
}

void PC::run()
{
    // Thread execution starts here for the PC
    // For Phase 1, PCs remain inactive
    // Implement PC's main loop in later phases
}
