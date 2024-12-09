#ifndef PC_H
#define PC_H

#include "Node.h"
#include <QSharedPointer>
#include "../Port/Port.h"

class PC : public Node
{
    Q_OBJECT

public:
    explicit PC(const QString &ipAddress, QObject *parent = nullptr);
    ~PC() override;

    PortPtr_t getPort();

    void startPC();
    void generatePacket();

protected:
    void run() override;

private:
    PortPtr_t m_port;
};

#endif // PC_H
