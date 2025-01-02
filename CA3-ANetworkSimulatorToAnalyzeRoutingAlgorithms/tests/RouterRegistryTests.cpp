#include <QtTest/QtTest>
#include <QSharedPointer>
#include "../src/Globals/RouterRegistry.h"
#include "../src/Network/Router.h"

class RouterRegistryTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testAddRouters();
    void testFindRouterById();
    void testFindRouterById_NotFound();
    void testDuplicateRouterIds();
};

void RouterRegistryTests::testAddRouters() {
    // Clear previous routers
    RouterRegistry::allRouters.clear();

    QSharedPointer<Router> router1 = QSharedPointer<Router>::create(1, "192.168.1.1");
    QSharedPointer<Router> router2 = QSharedPointer<Router>::create(2, "192.168.1.2");

    std::vector<QSharedPointer<Router>> routers = {router1, router2};
    RouterRegistry::addRouters(routers);

    QCOMPARE(RouterRegistry::allRouters.size(), static_cast<size_t>(2));
    QCOMPARE(RouterRegistry::allRouters[0]->getId(), 1);
    QCOMPARE(RouterRegistry::allRouters[1]->getId(), 2);
}

void RouterRegistryTests::testFindRouterById() {
    // Clear previous routers
    RouterRegistry::allRouters.clear();

    QSharedPointer<Router> router1 = QSharedPointer<Router>::create(1, "192.168.1.1");
    QSharedPointer<Router> router2 = QSharedPointer<Router>::create(2, "192.168.1.2");

    RouterRegistry::addRouters({router1, router2});

    auto foundRouter = RouterRegistry::findRouterById(1);
    QVERIFY(foundRouter != nullptr);
    QCOMPARE(foundRouter->getId(), 1);
    QCOMPARE(foundRouter->getIPAddress(), QString("192.168.1.1"));
}

void RouterRegistryTests::testFindRouterById_NotFound() {
    // Clear previous routers
    RouterRegistry::allRouters.clear();

    QSharedPointer<Router> router1 = QSharedPointer<Router>::create(1, "192.168.1.1");
    RouterRegistry::addRouters({router1});

    auto foundRouter = RouterRegistry::findRouterById(999); // Non-existing ID
    QVERIFY(foundRouter == nullptr);
}

void RouterRegistryTests::testDuplicateRouterIds() {
    // Clear previous routers
    RouterRegistry::allRouters.clear();

    QSharedPointer<Router> router1 = QSharedPointer<Router>::create(1, "192.168.1.1");
    QSharedPointer<Router> routerDuplicate = QSharedPointer<Router>::create(1, "192.168.1.100");

    RouterRegistry::addRouters({router1, routerDuplicate});

    auto foundRouter = RouterRegistry::findRouterById(1);
    QVERIFY(foundRouter != nullptr);
    QCOMPARE(foundRouter->getIPAddress(), QString("192.168.1.1")); // First added router should remain
}

// QTEST_MAIN(RouterRegistryTests)
#include "RouterRegistryTests.moc"
