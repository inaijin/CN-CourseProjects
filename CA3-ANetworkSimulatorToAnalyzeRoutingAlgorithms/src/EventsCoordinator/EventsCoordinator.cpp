#include "EventsCoordinator.h"
#include "../DataGenerator/DataGenerator.h"
#include <QDebug>

EventsCoordinator::EventsCoordinator(QThread *parent) :
    QThread {parent},
    m_timer {new QTimer(this)},
    m_dataGenerator {nullptr}  // Initialize DataGenerator pointer
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
EventsCoordinator::setDataGenerator(DataGenerator *generator)
{
    if (m_dataGenerator)
    {
        disconnect(m_dataGenerator, &DataGenerator::packetsGenerated, this, &EventsCoordinator::onPacketsGenerated);
    }

    m_dataGenerator = generator;

    if (m_dataGenerator)
    {
        connect(m_dataGenerator, &DataGenerator::packetsGenerated, this, &EventsCoordinator::onPacketsGenerated);
    }
}

void
EventsCoordinator::onTick()
{
    emit tick();
    qDebug() << "Tick emitted.";

    if (m_dataGenerator)
    {
        m_dataGenerator->generatePackets();
        qDebug() << "DataGenerator triggered to generate packets.";
    }
}

void
EventsCoordinator::onPacketsGenerated(const std::vector<QSharedPointer<Packet>> &packets)
{
    emit dataGenerated(packets);
    qDebug() << "Packets generated and forwarded. Count:" << packets.size();
}
