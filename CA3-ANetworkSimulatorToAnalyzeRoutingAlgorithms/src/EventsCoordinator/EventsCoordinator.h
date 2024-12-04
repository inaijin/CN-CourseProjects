#ifndef EVENTSCOORDINATOR_H
#define EVENTSCOORDINATOR_H

#include <chrono>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QSharedPointer>
#include <vector>

class DataGenerator;
class Packet;

class EventsCoordinator : public QThread
{
    Q_OBJECT

    typedef std::chrono::milliseconds Millis;

    explicit EventsCoordinator(QThread *parent = nullptr);

public:
    ~EventsCoordinator() override;
    static EventsCoordinator *instance(QThread *parent = nullptr);
    static void release();

    void startClock(Millis interval);
    void stopClock();

    void setDataGenerator(DataGenerator *generator);

Q_SIGNALS:
    void tick();
    void packetGenerated(QSharedPointer<Packet> packet);

private Q_SLOTS:
    void onTick();
    void onPacketsGenerated(const std::vector<QSharedPointer<Packet>> &packets);

private:
    inline static EventsCoordinator *m_self = nullptr;
    QTimer *m_timer = nullptr;
    DataGenerator *m_dataGenerator = nullptr;

    std::vector<QSharedPointer<Packet>> m_packetQueue;
};

#endif // EVENTSCOORDINATOR_H
