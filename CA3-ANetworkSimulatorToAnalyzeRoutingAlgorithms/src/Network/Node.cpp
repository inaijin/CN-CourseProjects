#include "Node.h"

int Node::s_nextId = 1;

Node::Node(const QString &ipAddress, NodeType type, QObject *parent)
    : QThread(parent),
    m_ipAddress(ipAddress),
    m_type(type)
{
    QMutexLocker locker(&m_mutex);
    m_id = s_nextId++;
}

Node::~Node()
{
    // Virtual destructor implementation
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
