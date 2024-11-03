#include <QGuiApplication>
#include <QQuickStyle>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Network/CallManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setApplicationName("WebRTC Voice Call");
    app.setApplicationVersion("1.0");

    QQuickStyle::setStyle("Fusion");

    QQmlApplicationEngine engine;

    CallManager callManager;

    engine.rootContext()->setContextProperty("callManager", &callManager);

    const QUrl url(QStringLiteral("qrc:/UI/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
