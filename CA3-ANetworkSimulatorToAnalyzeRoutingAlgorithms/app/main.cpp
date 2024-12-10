#include <QCoreApplication>
#include <QDir>
#include <QString>
#include "../src/NetworkSimulator/Simulator.h"
#include "../src/NetworkSimulator/ApplicationContext.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QString filePath = "../../../../GitHub/CN-CourseProjects/CA3-ANetworkSimulatorToAnalyzeRoutingAlgorithms/assets/config.json";

    QSharedPointer<Simulator> simulator = QSharedPointer<Simulator>::create();

    ApplicationContext::instance().setSimulator(simulator);

    if (!simulator->loadConfig(filePath))
    {
        qCritical() << "Failed to load configuration.";
        return -1;
    }

    simulator->initializeNetwork();
    simulator->startSimulation();

    return app.exec();
}

// Test Ha Update Shavad
