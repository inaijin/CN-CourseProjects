#ifndef PC_H
#define PC_H

#include <QObject>
#include <QSharedPointer>

#include "../Port/Port.h"

class PC : public QObject
{
    Q_OBJECT

public:
    explicit PC(int id, const QString &ipAddress, QObject *parent = nullptr);
    ~PC();

    int getId() const;
    QString getIPAddress() const;
    PortPtr_t getPort();

private:
    int m_id;
    QString m_ipAddress;
    PortPtr_t m_port;
};

#endif // PC_H
