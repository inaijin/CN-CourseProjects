#include <QCoreApplication>
#include <QDir>
#include <QString>
#include "../src/NetworkSimulator/Simulator.h"

int main(int argv, char* argc[])
{
    QCoreApplication app(argv, argc);
    QString filePath = "../../../../GitHub/CN-CourseProjects/CA3-ANetworkSimulatorToAnalyzeRoutingAlgorithms/assets/config.json";

    Simulator simulator;
    if (!simulator.loadConfig(filePath))
    {
        qCritical() << "Failed to load configuration.";
        return -1;
    }

    simulator.initializeNetwork();
    simulator.startSimulation();

    return app.exec();
}
