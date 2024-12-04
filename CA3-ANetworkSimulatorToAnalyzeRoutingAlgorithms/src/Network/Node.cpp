#include "Node.h"

Node::Node(int id, const QString &ipAddress, QObject *parent)
    : QThread(parent), m_id(id), m_ipAddress(ipAddress)
{
}

Node::~Node()
{
    // Virtual destructor implementation
}
