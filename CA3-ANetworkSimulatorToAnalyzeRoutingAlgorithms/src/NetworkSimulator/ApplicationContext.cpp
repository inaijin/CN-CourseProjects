#include "ApplicationContext.h"
#include <QDebug>

ApplicationContext& ApplicationContext::instance()
{
    static ApplicationContext instance;
    return instance;
}

void ApplicationContext::reset()
{
    // Optional method to reset the singleton, useful for testing
    ApplicationContext& ctx = instance();
    ctx.m_config.clear();
    ctx.m_simulator.reset();
}

ApplicationContext::ApplicationContext(QObject *parent)
    : QObject(parent)
{
    qDebug() << "ApplicationContext initialized.";
}

ApplicationContext::~ApplicationContext()
{
    qDebug() << "ApplicationContext destroyed.";
}

void ApplicationContext::setConfig(const QVariantMap& config)
{
    QWriteLocker locker(&m_configLock);
    m_config = config;
    qDebug() << "Configuration updated.";
}

QVariantMap ApplicationContext::getConfig() const
{
    QReadLocker locker(&m_configLock);
    return m_config;
}

void ApplicationContext::setSimulator(QSharedPointer<Simulator> simulator)
{
    QMutexLocker locker(&m_simulatorMutex);
    if (!simulator)
    {
        qWarning() << "Attempt to set a null simulator.";
    }
    m_simulator = simulator;
    qDebug() << "Simulator instance set.";
}

QSharedPointer<Simulator> ApplicationContext::getSimulator() const
{
    QMutexLocker locker(&m_simulatorMutex);
    if (!m_simulator)
    {
        qWarning() << "Simulator instance is null.";
    }
    return m_simulator;
}
