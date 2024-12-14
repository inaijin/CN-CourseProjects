#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QString>
#include <QMutex>

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
    virtual ~Node() = 0;

    int getId() const;
    QString getIPAddress() const;
    NodeType getNodeType() const;

    static int getNextGlobalId();

protected:
    // Removed QThread inheritance and run() method

    int m_id;
    QString m_ipAddress;
    NodeType m_type;
    mutable QMutex m_mutex;

    static int s_globalNodeId;
    static QMutex s_mutex; // Static mutex for thread-safe access to s_globalNodeId
};

#endif // NODE_H
