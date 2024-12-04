#include "EventsCoordinator.h"
#include <QDebug>

EventsCoordinator::EventsCoordinator(QThread *parent) :
    QThread {parent},
    m_timer {new QTimer(this)}
{
    connect(m_timer, &QTimer::timeout, this, &EventsCoordinator::onTick);
}

EventsCoordinator *
EventsCoordinator::instance(QThread *parent)
{
    if (!m_self)
    {
        m_self = new EventsCoordinator(parent);
    }

    return m_self;
}

void
EventsCoordinator::release()
{
    if (m_self)
    {
        delete m_self;
        m_self = nullptr;
    }
}

void
EventsCoordinator::startClock(Millis interval)
{
    if (!m_timer->isActive())
    {
        m_timer->start(interval.count());
        qDebug() << "Clock started with interval:" << interval.count() << "ms";
    }
}

void
EventsCoordinator::stopClock()
{
    if (m_timer->isActive())
    {
        m_timer->stop();
        qDebug() << "Clock stopped.";
    }
}

void
EventsCoordinator::onTick()
{
    emit tick();
    qDebug() << "Tick emitted.";
}
