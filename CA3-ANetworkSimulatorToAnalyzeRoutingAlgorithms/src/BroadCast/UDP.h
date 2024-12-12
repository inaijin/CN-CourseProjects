#ifndef UDP_H
#define UDP_H

#include <QObject>
#include "../Packet/Packet.h"
#include "../Network/Router.h"

class UDP : public QObject
{
    Q_OBJECT

public:
    explicit UDP(QObject *parent = nullptr);
    ~UDP();

    void broadcastPacket(const PacketPtr_t &packet, const QSharedPointer<Router> &sourceRouter);
};

#endif // UDP_H
