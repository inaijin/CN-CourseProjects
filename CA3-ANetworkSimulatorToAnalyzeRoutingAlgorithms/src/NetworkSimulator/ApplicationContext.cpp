#include "ApplicationContext.h"

ApplicationContext& ApplicationContext::instance()
{
    static ApplicationContext instance;
    return instance;
}

ApplicationContext::ApplicationContext(QObject *parent)
    : QObject(parent)
{
    // Initialization code if necessary
}

ApplicationContext::~ApplicationContext()
{
    // Cleanup code if necessary
}

void ApplicationContext::setConfig(const QVariantMap& config)
{
    QMutexLocker locker(&m_mutex);
    m_config = config;
}

QVariantMap ApplicationContext::getConfig() const
{
    QMutexLocker locker(&m_mutex);
    return m_config;
}

void ApplicationContext::setSimulator(QSharedPointer<Simulator> simulator)
{
    QMutexLocker locker(&m_mutex);
    m_simulator = simulator;
}

QSharedPointer<Simulator> ApplicationContext::getSimulator() const
{
    QMutexLocker locker(&m_mutex);
    return m_simulator;
}
