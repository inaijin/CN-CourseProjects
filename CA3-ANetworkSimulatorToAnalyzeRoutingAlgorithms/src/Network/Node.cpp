#include "Node.h"

Node::Node(int id, const QString &ipAddress, NodeType type, QObject *parent)
    : QThread(parent), m_id(id), m_ipAddress(ipAddress), m_type(type)
{
}

int Node::getId() const
{
    QMutexLocker locker(&m_mutex);
    return m_id;
}

QString Node::getIPAddress() const
{
    QMutexLocker locker(&m_mutex);
    return m_ipAddress;
}

NodeType Node::getNodeType() const
{
    QMutexLocker locker(&m_mutex);
    return m_type;
}

Node::~Node()
{
    // Virtual destructor implementation
}
