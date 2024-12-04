#include "PortBindingManager.h"
#include <QDebug>

PortBindingManager::PortBindingManager(QObject *parent) :
    QObject {parent}
{}

void
PortBindingManager::bind(const PortPtr_t &port1, const PortPtr_t &port2)
{
    if (!port1 || !port2)
    {
        qWarning() << "Invalid ports provided for binding.";
        return;
    }

    if (port1->isConnected() || port2->isConnected())
    {
        qWarning() << "One of the ports is already connected.";
        return;
    }

    connect(port1.data(), &Port::packetSent, port2.data(), &Port::receivePacket);
    connect(port2.data(), &Port::packetSent, port1.data(), &Port::receivePacket);

    port1->setConnected(true);
    port2->setConnected(true);

    m_bindings.insert(port1, port2);
    m_bindings.insert(port2, port1);

    emit bindingChanged(port1->getRouterIP(), port1->getPortNumber(),
                        port2->getRouterIP(), port2->getPortNumber(), true);

    qDebug() << "Ports bound between Router" << port1->getRouterIP() << "Port" << port1->getPortNumber()
             << "and Router" << port2->getRouterIP() << "Port" << port2->getPortNumber();
}

bool
PortBindingManager::unbind(const PortPtr_t &port1, const PortPtr_t &port2)
{
    if (!port1 || !port2)
    {
        qWarning() << "Invalid ports provided for unbinding.";
        return false;
    }

    if (!m_bindings.contains(port1) || m_bindings.value(port1) != port2)
    {
        qWarning() << "Ports are not bound.";
        return false;
    }

    disconnect(port1.data(), &Port::packetSent, port2.data(), &Port::receivePacket);
    disconnect(port2.data(), &Port::packetSent, port1.data(), &Port::receivePacket);

    port1->setConnected(false);
    port2->setConnected(false);

    m_bindings.remove(port1);
    m_bindings.remove(port2);

    emit bindingChanged(port1->getRouterIP(), port1->getPortNumber(),
                        port2->getRouterIP(), port2->getPortNumber(), false);

    qDebug() << "Ports unbound between Router" << port1->getRouterIP() << "Port" << port1->getPortNumber()
             << "and Router" << port2->getRouterIP() << "Port" << port2->getPortNumber();

    return true;
}
