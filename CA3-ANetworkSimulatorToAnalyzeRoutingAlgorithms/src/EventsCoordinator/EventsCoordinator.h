#ifndef EVENTSCOORDINATOR_H
#define EVENTSCOORDINATOR_H

#include <chrono>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QSharedPointer>
#include <vector>
#include "../Network/Router.h"
#include "../Network/PC.h"

class DataGenerator;
class Packet;

class EventsCoordinator : public QThread
{
    Q_OBJECT

    typedef std::chrono::milliseconds Millis;

protected:
    explicit EventsCoordinator(QThread *parent = nullptr);

public:
    explicit EventsCoordinator(QObject *parent = nullptr);
    ~EventsCoordinator() override;
    static EventsCoordinator *instance(QThread *parent = nullptr);
    static void release();

    void startClock(Millis interval);
    void stopClock();

    void setDataGenerator(DataGenerator *generator);
    void addRouter(const QSharedPointer<Router> &router);

protected:
    void run() override;

signals:
    void tick();
    void packetGenerated(QSharedPointer<Packet> packet);
    void convergenceDetected();

private slots:
    void onTick();
    void onPacketsGenerated(const std::vector<QSharedPointer<Packet>> &packets);
    void onRoutingTableUpdated(int routerId);

private:
    inline static EventsCoordinator *m_self = nullptr;
    QTimer *m_timer = nullptr;
    DataGenerator *m_dataGenerator = nullptr;

    std::vector<QSharedPointer<Packet>> m_packetQueue;
    std::vector<QSharedPointer<Router>> m_routers;
    std::vector<QSharedPointer<PC>> m_pcs;

    bool m_routingChangedThisTick;
    int m_convergenceTickCounter;
    const int REQUIRED_STABLE_TICKS = 20;

    void synchronizeRoutersWithDHCP();
    int m_currentTime = 0;
};

#endif // EVENTSCOORDINATOR_H
