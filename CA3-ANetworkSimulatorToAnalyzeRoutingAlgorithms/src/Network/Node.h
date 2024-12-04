#ifndef NODE_H
#define NODE_H

#include <QThread>
#include <QObject>
#include <QString>

class Node : public QThread
{
    Q_OBJECT

public:
    explicit Node(int id, const QString &ipAddress, QObject *parent = nullptr);
    virtual ~Node() = 0;

    int getId() const;
    QString getIPAddress() const;

protected:
    virtual void run() override = 0;

    int m_id;
    QString m_ipAddress;
};

#endif // NODE_H
