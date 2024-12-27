#include "Node.h"
#include "../IP/IP.h"

int Node::s_globalNodeId = 0;

Node::Node(int id, const QString &ipAddress, NodeType type, QObject *parent)
    : QObject(parent), m_id(id), m_ipAddress(QSharedPointer<IP>::create(ipAddress)), m_type(type)
{

}

Node::~Node() {}

int Node::getNextGlobalId()
{
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
    return m_ipAddress->getIp();
}

NodeType Node::getNodeType() const
{
    QMutexLocker locker(&m_mutex);
    return m_type;
}
