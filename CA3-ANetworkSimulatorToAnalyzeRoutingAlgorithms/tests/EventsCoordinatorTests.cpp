#include <QtTest/QtTest>
#include "../src/EventsCoordinator/EventsCoordinator.h"

class EventsCoordinatorTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testSingletonInstance();
    void testStartClock();
    void testStopClock();
    void testTickSignal();
};

void EventsCoordinatorTests::testSingletonInstance() {
    EventsCoordinator *instance1 = EventsCoordinator::instance();
    EventsCoordinator *instance2 = EventsCoordinator::instance();
    QCOMPARE(instance1, instance2);

    EventsCoordinator::release();
}

void EventsCoordinatorTests::testStartClock() {
    EventsCoordinator *coordinator = EventsCoordinator::instance();

    QSignalSpy spy(coordinator, &EventsCoordinator::tick);
    coordinator->startClock(std::chrono::milliseconds(100));

    QTest::qWait(350);

    QVERIFY(spy.count() >= 3);

    coordinator->stopClock();
    EventsCoordinator::release();
}

void EventsCoordinatorTests::testStopClock() {
    EventsCoordinator *coordinator = EventsCoordinator::instance();

    QSignalSpy spy(coordinator, &EventsCoordinator::tick);
    coordinator->startClock(std::chrono::milliseconds(100));

    QTest::qWait(150);

    coordinator->stopClock();
    int previousCount = spy.count();
    QTest::qWait(150);

    QCOMPARE(spy.count(), previousCount);

    EventsCoordinator::release();
}

void EventsCoordinatorTests::testTickSignal() {
    // Test the actual implementation of what happens on tick
    // Not implemented yet, add this later once the tick-handling mechanism is defined
    // Example:
    // - Ensure that other components (e.g., DataGenerator) are notified correctly
    // - Verify that the system reacts appropriately to tick events
}

QTEST_MAIN(EventsCoordinatorTests)
#include "EventsCoordinatorTests.moc"
