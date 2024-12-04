#include "Node.h"

Node::Node(int id, const QString &ipAddress, QObject *parent)
    : QThread(parent), m_id(id), m_ipAddress(ipAddress)
{
}

int Node::getId() const
{
    return m_id;
}

QString Node::getIPAddress() const
{
    return m_ipAddress;
}


Node::~Node()
{
    // Virtual destructor implementation
}
