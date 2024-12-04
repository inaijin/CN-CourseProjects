#ifndef EVENTSCOORDINATOR_H
#define EVENTSCOORDINATOR_H

#include <chrono>
#include <QObject>
#include <QThread>
#include <QTimer>

class EventsCoordinator : public QThread
{
    Q_OBJECT

    typedef std::chrono::milliseconds Millis;

    explicit EventsCoordinator(QThread *parent = nullptr);

public:
    ~EventsCoordinator() override = default;
    static EventsCoordinator *instance(QThread *parent = nullptr);
    static void               release();

public:
    void startClock(Millis interval);
    void stopClock();

Q_SIGNALS:
    void tick();

private Q_SLOTS:
    void onTick();

private:
    inline static EventsCoordinator *m_self = nullptr;
    QTimer *m_timer = nullptr;
};

#endif
