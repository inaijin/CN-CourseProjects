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

EventsCoordinator::~EventsCoordinator() {
    QMetaObject::invokeMethod(this, [this]() {
        if (m_timer) {
            m_timer->stop();
            delete m_timer;
            m_timer = nullptr;
        }
    });

    quit();
    wait();
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

void EventsCoordinator::startClock(Millis interval) {
    QMetaObject::invokeMethod(this, [this, interval]() {
        if (!m_timer) {
            m_timer = new QTimer();
            connect(m_timer, &QTimer::timeout, this, &EventsCoordinator::onTick);
        }
        m_timer->start(interval.count());
        qDebug() << "Clock started with interval:" << interval.count() << "ms";
    });
}

void EventsCoordinator::stopClock() {
    QMetaObject::invokeMethod(this, [this]() {
        if (m_timer && m_timer->isActive()) {
            m_timer->stop();
            qDebug() << "Clock stopped.";
        }
    });
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
        m_dataGenerator->generatePackets();
    }
}

void EventsCoordinator::onTick() {
    ++m_currentTime;
    emit tick();
    qDebug() << "Tick emitted. Current time:" << m_currentTime;

    for (const auto &router : m_routers) {
        if (router->isDHCPServer()) {
            router->getDHCPServer()->tick(m_currentTime);
        }
    }

    if (m_packetQueue.empty()) {
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

void EventsCoordinator::addRouter(const QSharedPointer<Router> &router) {
    m_routers.push_back(router);
    connect(this, &EventsCoordinator::tick, router.data(), &Router::requestIPFromDHCP);
}

void EventsCoordinator::run() {
    m_timer = new QTimer();
    connect(m_timer, &QTimer::timeout, this, &EventsCoordinator::onTick);
    m_timer->start(1000);
    qDebug() << "Clock started with interval:" << 1000 << "ms";

    exec();
    m_timer->stop();
    delete m_timer;
    m_timer = nullptr;
}
