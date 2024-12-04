#include <QtTest/QtTest>
#include <QSignalSpy>
#include "../src/EventsCoordinator/EventsCoordinator.h"
#include "../src/DataGenerator/DataGenerator.h"

class EventsCoordinatorTests : public QObject {
    Q_OBJECT

private:
    DataGenerator *m_dataGenerator;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testSingletonInstance();
    void testStartClock();
    void testStopClock();
    void testTickSignal();
    void testDataGeneration();
};

void EventsCoordinatorTests::initTestCase() {
    m_dataGenerator = new DataGenerator();
    m_dataGenerator->setLambda(5.0);
    m_dataGenerator->setDestinations({"PC1", "PC2", "Router1"});
}

void EventsCoordinatorTests::cleanupTestCase() {
    delete m_dataGenerator;
    EventsCoordinator::release();
}

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
    EventsCoordinator *coordinator = EventsCoordinator::instance();

    QSignalSpy tickSpy(coordinator, &EventsCoordinator::tick);
    coordinator->startClock(std::chrono::milliseconds(200));

    QTest::qWait(650);
    QVERIFY(tickSpy.count() >= 3);

    coordinator->stopClock();
    EventsCoordinator::release();
}

void EventsCoordinatorTests::testDataGeneration() {
    EventsCoordinator *coordinator = EventsCoordinator::instance();
    coordinator->setDataGenerator(m_dataGenerator);

    QSignalSpy dataSpy(coordinator, &EventsCoordinator::dataGenerated);
    coordinator->startClock(std::chrono::milliseconds(100));

    QTest::qWait(350);

    QVERIFY(dataSpy.count() > 0);

    QList<QVariant> firstSignal = dataSpy.takeFirst();
    auto packets = firstSignal[0].value<std::vector<QSharedPointer<Packet>>>();
    QVERIFY(!packets.empty());

    coordinator->stopClock();
    EventsCoordinator::release();
}

QTEST_MAIN(EventsCoordinatorTests)
#include "EventsCoordinatorTests.moc"
