#ifndef PC_H
#define PC_H

#include "Node.h"
#include <QSharedPointer>
#include "../Port/Port.h"

class PC : public Node
{
    Q_OBJECT

public:
    explicit PC(int id, const QString &ipAddress, QObject *parent = nullptr);
    ~PC();

    PortPtr_t getPort();

protected:
    void run() override;

private:
    PortPtr_t m_port;
};

#endif // PC_H
