// #include <QtTest/QtTest>
// #include "../src/Port/Port.h"
// #include "../src/PortBindingManager/PortBindingManager.h"

// class PortBindingManagerTests : public QObject {
//     Q_OBJECT

// private Q_SLOTS:
//     void testPortBinding();
//     void testPortUnbinding();
//     void testInvalidBinding();
// };

// void PortBindingManagerTests::testPortBinding() {
//     PortBindingManager manager;
//     PortPtr_t port1 = QSharedPointer<Port>::create();
//     PortPtr_t port2 = QSharedPointer<Port>::create();

//     port1->setPortNumber(1);
//     port1->setRouterIP("192.168.1.1");
//     port2->setPortNumber(2);
//     port2->setRouterIP("192.168.1.2");

//     manager.bind(port1, port2);

//     QVERIFY(port1->isConnected());
//     QVERIFY(port2->isConnected());
// }

// void PortBindingManagerTests::testPortUnbinding() {
//     PortBindingManager manager;
//     PortPtr_t port1 = QSharedPointer<Port>::create();
//     PortPtr_t port2 = QSharedPointer<Port>::create();

//     port1->setPortNumber(1);
//     port1->setRouterIP("192.168.1.1");
//     port2->setPortNumber(2);
//     port2->setRouterIP("192.168.1.2");

//     manager.bind(port1, port2);
//     QVERIFY(manager.unbind(port1, port2));

//     QVERIFY(!port1->isConnected());
//     QVERIFY(!port2->isConnected());
// }

// void PortBindingManagerTests::testInvalidBinding() {
//     PortBindingManager manager;
//     PortPtr_t port1 = QSharedPointer<Port>::create();
//     PortPtr_t port2 = QSharedPointer<Port>::create();

//     port1->setPortNumber(1);
//     port1->setRouterIP("192.168.1.1");

//     manager.bind(port1, nullptr);
//     QVERIFY(!port1->isConnected());

//     manager.bind(nullptr, port2);
//     QVERIFY(!port2->isConnected());
// }

// // QTEST_MAIN(PortBindingManagerTests)
// #include "PortBindingManagerTests.moc"
