#ifndef APPLICATIONCONTEXT_H
#define APPLICATIONCONTEXT_H

#include <QObject>
#include <QReadWriteLock>
#include <QMutex>
#include <QSharedPointer>
#include <QVariantMap>
#include <QString>
#include "../NetworkSimulator/Simulator.h"

class ApplicationContext : public QObject
{
    Q_OBJECT

public:
    static ApplicationContext& instance();

    void setConfig(const QVariantMap& config);
    QVariantMap getConfig() const;

    void setSimulator(QSharedPointer<Simulator> simulator);
    QSharedPointer<Simulator> getSimulator() const;

    static void reset();

private:
    explicit ApplicationContext(QObject *parent = nullptr);
    ~ApplicationContext();

    ApplicationContext(const ApplicationContext&) = delete;
    ApplicationContext& operator=(const ApplicationContext&) = delete;

    QVariantMap m_config;
    QSharedPointer<Simulator> m_simulator;

    mutable QReadWriteLock m_configLock; // ReadWriteLock for config
    mutable QMutex m_simulatorMutex;    // Mutex for simulator access
};

#endif // APPLICATIONCONTEXT_H
