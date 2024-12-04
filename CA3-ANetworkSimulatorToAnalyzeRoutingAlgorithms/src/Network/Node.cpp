#include "Node.h"

Node::Node(int id, const QString &ipAddress, QObject *parent)
    : QThread(parent), m_id(id), m_ipAddress(ipAddress)
{
    // Common initialization if existed for both Node and PC
}

Node::~Node()
{
    // Cleanup if necessary
}

int Node::getId() const
{
    return m_id;
}

QString Node::getIPAddress() const
{
    return m_ipAddress;
}
