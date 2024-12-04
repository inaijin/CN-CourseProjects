#include <QCoreApplication>
#include <QDir>
#include "../src/NetworkSimulator/Simulator.h"

int main(int argv, char* argc[])
{

    QCoreApplication app(argv, argc);

    qDebug() << "Current working directory:" << QDir::currentPath();

    Simulator simulator;
    if (!simulator.loadConfig("../assets/config.json"))
    {
        qCritical() << "Failed to load configuration.";
        return -1;
    }

    simulator.initializeNetwork();
    simulator.startSimulation();

    return app.exec();
}
