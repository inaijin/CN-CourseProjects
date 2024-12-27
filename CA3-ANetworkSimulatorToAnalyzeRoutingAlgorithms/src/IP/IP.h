#ifndef IP_H
#define IP_H

#include "IPHeader.h"
#include <QObject>
#include <QSharedPointer>

class IP : public QObject
{
    Q_OBJECT

public:
    explicit IP(const QSharedPointer<AbstractIPHeader> &header, QObject *parent = nullptr);
    explicit IP(const QString &ip, QObject *parent = nullptr);
    ~IP();

    QSharedPointer<AbstractIPHeader> getHeader() const;

    void setIp(QString IP) { m_ip = IP; }
    QString getIp() const { return m_ip; }

private:
    QSharedPointer<AbstractIPHeader> m_header;
    QString m_ip;
};

#endif // IP_H
