#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>

enum LogLevel {
    ERROR_LEVEL,
    WARNING_LEVEL,
    DEBUG_LEVEL
};

constexpr LogLevel CURRENT_LOG_LEVEL = WARNING_LEVEL;

#if CURRENT_LOG_LEVEL >= DEBUG_LEVEL
    #define LOG_DEBUG() qDebug()
#else
    #define LOG_DEBUG() QNoDebug()
#endif

#if CURRENT_LOG_LEVEL >= WARNING_LEVEL
    #define LOG_WARNING() qWarning()
#else
    #define LOG_WARNING() QNoDebug()
#endif

#if CURRENT_LOG_LEVEL >= ERROR_LEVEL
    #define LOG_ERROR() qCritical()
#else
    #define LOG_ERROR() QNoDebug()
#endif

#endif // LOGGER_H
