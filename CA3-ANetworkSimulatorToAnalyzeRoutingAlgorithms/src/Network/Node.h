#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QString>
#include <QMutex>
#include "../MACAddress/MACAddress.h"
#include "../IP/IP.h"

enum class NodeType
{
    PC,
    Router
};

class Node : public QObject
{
    Q_OBJECT

public:
    explicit Node(int id, const QString &ipAddress, NodeType type, QObject *parent = nullptr);
    virtual ~Node();

    int getId() const;
    QString getIPAddress() const;
    NodeType getNodeType() const;
    void setMacAddress(MACAddress macAddr) { m_macAddress = macAddr; }

    static int getNextGlobalId();

protected:
    int m_id;
    QSharedPointer<IP> m_ipAddress;
    NodeType m_type;
    MACAddress m_macAddress;

    mutable QMutex m_mutex;

    static int s_globalNodeId;
};

#endif // NODE_H
