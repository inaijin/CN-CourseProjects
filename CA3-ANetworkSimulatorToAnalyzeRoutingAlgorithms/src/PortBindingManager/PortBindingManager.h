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

    void bind(const QSharedPointer<Port> &port1, const QSharedPointer<Port> &port2, int router1Id, int router2Id);
    bool unbind(const QSharedPointer<Port> &port1, const QSharedPointer<Port> &port2);

    bool isBound(const QSharedPointer<Port> &port) const;

Q_SIGNALS:
    void bindingChanged(int router1Id, uint8_t port1, int router2Id, uint8_t port2, bool bind);

private:
    QMap<PortPtr_t, PortPtr_t> m_bindings;
    mutable QMutex m_mutex;
};

#endif // PORTBINDINGMANAGER_H
