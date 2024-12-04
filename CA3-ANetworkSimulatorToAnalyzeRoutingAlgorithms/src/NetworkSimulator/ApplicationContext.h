#ifndef APPLICATIONCONTEXT_H
#define APPLICATIONCONTEXT_H

#include <QObject>
#include <QMutex>
#include <QSharedPointer>
#include <QVariantMap>
#include <QString>
#include "../NetworkSimulator/Simulator.h" // Include your Simulator header

class ApplicationContext : public QObject
{
    Q_OBJECT

public:
    static ApplicationContext& instance();

    // Configuration management
    void setConfig(const QVariantMap& config);
    QVariantMap getConfig() const;

    // Access to global resources
    void setSimulator(QSharedPointer<Simulator> simulator);
    QSharedPointer<Simulator> getSimulator() const;

    // Add other global resources as needed

private:
    explicit ApplicationContext(QObject *parent = nullptr);
    ~ApplicationContext();

    // Disable copy constructor and assignment operator
    ApplicationContext(const ApplicationContext&) = delete;
    ApplicationContext& operator=(const ApplicationContext&) = delete;

    // Member variables
    QVariantMap m_config;
    QSharedPointer<Simulator> m_simulator;

    mutable QMutex m_mutex; // Mutex for thread safety
};

#endif // APPLICATIONCONTEXT_H
