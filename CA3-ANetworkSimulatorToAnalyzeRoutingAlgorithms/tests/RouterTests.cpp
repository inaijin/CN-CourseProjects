#include <QtTest/QtTest>
#include "../Network/Router.h"

class RouterTests : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testRouterInitialization();
};

void RouterTests::testRouterInitialization() {
    Router router(1, "192.168.1.1", 4);
    QCOMPARE(router.getId(), 1);
    QCOMPARE(router.getIPAddress(), QString("192.168.1.1"));
    QCOMPARE(static_cast<int>(router.getPorts().size()), 4);
}

// QTEST_MAIN(RouterTests)
#include "RouterTests.moc"
