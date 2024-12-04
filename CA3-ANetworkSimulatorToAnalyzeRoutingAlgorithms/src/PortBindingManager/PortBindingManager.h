#ifndef PORTBINDINGMANAGER_H
#define PORTBINDINGMANAGER_H

#include "../Port/Port.h"

#include <QMap>
#include <QMutex>
#include <QObject>

class PortBindingManager : public QObject
{
    Q_OBJECT

public:
    explicit PortBindingManager(QObject *parent = nullptr);
    ~PortBindingManager() override = default;

    void bind(const PortPtr_t &port1, const PortPtr_t &port2);
    bool unbind(const PortPtr_t &port1, const PortPtr_t &port2);

Q_SIGNALS:
    void bindingChanged(const QString &router1, uint8_t port1, const QString &router2, uint8_t port2, bool bind);

private:
    QMap<PortPtr_t, PortPtr_t> m_bindings;
    mutable QMutex m_mutex;
};

#endif    // PORTBINDINGMANAGER_H
