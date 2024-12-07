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
    ~IP();

    QSharedPointer<AbstractIPHeader> getHeader() const;

private:
    QSharedPointer<AbstractIPHeader> m_header;
};

#endif // IP_H
