#include <QtTest/QtTest>
#include "../src/EventsCoordinator/EventsCoordinator.h"
#include "../src/DataGenerator/DataGenerator.h"

class EventsCoordinatorTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testSingletonInstance();
    void testStartAndStopClock();
    void testTickSignalEmission();
    void testPacketGenerationIntegration();
};

void EventsCoordinatorTests::testSingletonInstance() {
    EventsCoordinator *instance1 = EventsCoordinator::instance();
    EventsCoordinator *instance2 = EventsCoordinator::instance();

    QCOMPARE(instance1, instance2);
    EventsCoordinator::release();
}

void EventsCoordinatorTests::testStartAndStopClock() {
    EventsCoordinator *coordinator = EventsCoordinator::instance();

    QSignalSpy spy(coordinator, &EventsCoordinator::tick);
    coordinator->startClock(std::chrono::milliseconds(100));

    QTest::qWait(350);
    coordinator->stopClock();

    QVERIFY(spy.count() >= 3);
    EventsCoordinator::release();
}

void EventsCoordinatorTests::testTickSignalEmission() {
    EventsCoordinator *coordinator = EventsCoordinator::instance();

    QSignalSpy spy(coordinator, &EventsCoordinator::tick);
    coordinator->startClock(std::chrono::milliseconds(200));

    QTest::qWait(500);
    QCOMPARE(spy.count(), 2);

    coordinator->stopClock();
    EventsCoordinator::release();
}

void EventsCoordinatorTests::testPacketGenerationIntegration() {
    EventsCoordinator *coordinator = EventsCoordinator::instance();
    DataGenerator generator;

    std::vector<QString> destinations = {"192.168.1.1", "192.168.1.2"};
    generator.setDestinations(destinations);
    generator.setLambda(3.0);

    coordinator->setDataGenerator(&generator);

    QSignalSpy tickSpy(coordinator, &EventsCoordinator::tick);
    QSignalSpy packetSpy(coordinator, &EventsCoordinator::dataGenerated);

    coordinator->startClock(std::chrono::milliseconds(100));
    QTest::qWait(350);

    QVERIFY(tickSpy.count() >= 3);
    QVERIFY(packetSpy.count() > 0);

    coordinator->stopClock();
    EventsCoordinator::release();
}

// QTEST_MAIN(EventsCoordinatorTests)
#include "EventsCoordinatorTests.moc"
