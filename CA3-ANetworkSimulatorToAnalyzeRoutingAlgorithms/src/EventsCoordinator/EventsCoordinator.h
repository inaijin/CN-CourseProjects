#ifndef EVENTSCOORDINATOR_H
#define EVENTSCOORDINATOR_H

#include <chrono>
#include <QObject>
#include <QTimer>
#include <QSharedPointer>
#include <vector>
#include "../Network/Router.h"

class DataGenerator;
class Packet;

class EventsCoordinator : public QObject
{
    Q_OBJECT

    typedef std::chrono::milliseconds Millis;

public:
    // Singleton access methods
    static EventsCoordinator* instance(QObject *parent = nullptr);
    static void release();

    explicit EventsCoordinator(QObject *parent = nullptr);
    ~EventsCoordinator() override;

    void startClock(Millis interval);
    void stopClock();

    void setDataGenerator(DataGenerator *generator);
    void addRouter(const QSharedPointer<Router> &router);

Q_SIGNALS:
    void tick();
    void packetGenerated(QSharedPointer<Packet> packet);

private Q_SLOTS:
    void onTick();
    void onPacketsGenerated(const std::vector<QSharedPointer<Packet>> &packets);

private:
    static EventsCoordinator *m_self;
    QTimer *m_timer;
    DataGenerator *m_dataGenerator;

    std::vector<QSharedPointer<Packet>> m_packetQueue;
    std::vector<QSharedPointer<Router>> m_routers;

    void synchronizeRoutersWithDHCP();
    int m_currentTime;

    // Prevent copying
    EventsCoordinator(const EventsCoordinator &) = delete;
    EventsCoordinator& operator=(const EventsCoordinator &) = delete;
};

#endif // EVENTSCOORDINATOR_H
