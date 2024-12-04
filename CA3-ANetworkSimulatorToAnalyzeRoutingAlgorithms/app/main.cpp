#include <QCoreApplication>
#include "../src/NetworkSimulator/Simulator.h"

int main(int argv, char* argc[])
{

    QCoreApplication app(argv, argc);

    Simulator simulator;
    if (!simulator.loadConfig("config.json"))
    {
        qCritical() << "Failed to load configuration.";
        return -1;
    }

    simulator.initializeNetwork();
    simulator.startSimulation();

    return app.exec();
}
