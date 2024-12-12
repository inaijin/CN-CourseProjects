#include "UDP.h"
#include "../Port/Port.h"
#include <QDebug>

UDP::UDP(QObject *parent) : QObject(parent) {}

UDP::~UDP() {}

void UDP::broadcastPacket(const PacketPtr_t &packet, const QSharedPointer<Router> &sourceRouter) {
    if (!packet || !sourceRouter) {
        qWarning() << "Invalid packet or source router for broadcasting.";
        return;
    }

    const auto &ports = sourceRouter->getPorts();
    for (const auto &port : ports) {
        if (port->isConnected()) {
            port->sendPacket(packet);
            qDebug() << "Broadcasted packet from Router" << sourceRouter->getId() << "via Port" << port->getPortNumber();
        } else {
            qDebug() << "Port" << port->getPortNumber() << "on Router" << sourceRouter->getId() << "is not connected.";
        }
    }
}
