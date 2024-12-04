#include "EventsCoordinator.h"
#include "../DataGenerator/DataGenerator.h"
#include <QDebug>

EventsCoordinator::EventsCoordinator(QThread *parent) :
    QThread {parent},
    m_timer {new QTimer(this)},
    m_dataGenerator {nullptr}
{
    connect(m_timer, &QTimer::timeout, this, &EventsCoordinator::onTick);
}

EventsCoordinator::~EventsCoordinator()
{
    if (m_timer->isActive())
    {
        m_timer->stop();
    }
    delete m_timer;
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
        m_dataGenerator->generatePackets(); // Preload packets into the queue
    }
}

void
EventsCoordinator::onTick()
{
    emit tick();
    qDebug() << "Tick emitted.";

    if (m_packetQueue.empty())
    {
        qDebug() << "No packets left to emit.";
        return;
    }

    auto packet = m_packetQueue.front();
    m_packetQueue.erase(m_packetQueue.begin());
    emit packetGenerated(packet);

    qDebug() << "Packet emitted from the queue.";
}

void
EventsCoordinator::onPacketsGenerated(const std::vector<QSharedPointer<Packet>> &packets)
{
    m_packetQueue.insert(m_packetQueue.end(), packets.begin(), packets.end());
    qDebug() << packets.size() << "packets added to the queue. Total queue size:" << m_packetQueue.size();
}
