#include <QDir>
#include <QString>
#include <QCoreApplication>

#include "../src/NetworkSimulator/Simulator.h"
#include "../src/NetworkSimulator/ApplicationContext.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QString filePath = ":/configs/mainConfig/config.json";

    QSharedPointer<Simulator> simulator = QSharedPointer<Simulator>::create();

    ApplicationContext::instance().setSimulator(simulator);

    if (!simulator->loadConfig(filePath))
    {
        qCritical() << "Failed to load configuration.";
        return -1;
    }

    QStringList arguments = app.arguments();
    if (!simulator->configureFromCommandLine(arguments))
    {
        qCritical() << "Failed to configure simulator.";
        return -1;
    }

    simulator->initializeNetwork();
    simulator->startSimulation();

    return app.exec();
}
