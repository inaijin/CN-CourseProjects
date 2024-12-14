#include "Node.h"

int Node::s_globalNodeId = 0;
QMutex Node::s_mutex; // Define the static mutex

Node::Node(int id, const QString &ipAddress, NodeType type, QObject *parent)
    : QObject(parent), m_id(id), m_ipAddress(ipAddress), m_type(type)
{ }

Node::~Node() {}

int Node::getNextGlobalId()
{
    QMutexLocker locker(&s_mutex); // Use static mutex to protect s_globalNodeId
    return ++s_globalNodeId;
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
