#ifndef NODE_H
#define NODE_H

#include <QThread>
#include <QObject>
#include <QString>
#include <QMutex>

enum class NodeType
{
    PC,
    Router
};

class Node : public QThread
{
    Q_OBJECT

public:
    explicit Node(const QString &ipAddress, NodeType type, QObject *parent = nullptr);
    virtual ~Node() = 0;

    int getId() const;
    QString getIPAddress() const;
    NodeType getNodeType() const;

protected:
    virtual void run() override = 0;

    int m_id;
    QString m_ipAddress;
    NodeType m_type;
    mutable QMutex m_mutex;

private:
    static int s_nextId;
};

#endif // NODE_H
