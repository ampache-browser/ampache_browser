// logging.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef LOGGING_H
#define LOGGING_H



#include "infrastructure/logging/qt_logger/qt_logger.h"



namespace infrastructure {

/**
 * @brief Sets the maximal loggin level.
 *
 * If a log method with lower level than maxLogLevel is called, no message is logged.
 *
 * Usage example:
 * @code
 * SET_LOG_LEVEL(LogLevel::Debug);
 * @endcode
 *
 * @sa LogLevel, LOG_DBG, LOG_INF, LOG_WARN, LOG_ERR
 */
#define SET_LOG_LEVEL QtLogger::setMaxLogLevel

/**
 * @brief Logs an error (LogLevel::Error) message.
 *
 * Usage example:
 * @code
 * LOG_ERR("An error nr. %d has occured.", errorNumber);
 * @endcode
 */
#define LOG_ERR QtLogger(__FILE__, __LINE__, Q_FUNC_INFO).error

/**
 * @brief Logs a warning (LogLevel::Warning) message.
 *
 * Usage example:
 * @code
 * LOG_WARN("Value of 'x' (%d) is too small.", x);
 * @endcode
 */
#define LOG_WARN QtLogger(__FILE__, __LINE__, Q_FUNC_INFO).warning

/**
 * @brief Logs an info (LogLevel::Info) message.
 *
 * Usage example:
 * @code
 * LOG_INF("Value of 'y' is %d.", y);
 * @endcode
 */
#define LOG_INF QtLogger(__FILE__, __LINE__, Q_FUNC_INFO).info

#ifdef AMPACHE_BROWSER_DEBUG

/**
 * @brief Logs a debug (LogLevel::Debug) message.
 *
 * Usage example:
 * @code
 * LOG_DBG("Value of 'tmp' is '%s'.", tmp);
 * @endcode
 */
#define LOG_DBG QtLogger(__FILE__, __LINE__, Q_FUNC_INFO).debug

#else

/**
 * @brief Does nothing (release build)
 *
 * Usage example:
 * @code
 * LOG_DBG("Value of 'tmp' is '%s'.", tmp);
 * @endcode
 */
#define LOG_DBG __noopLog

#endif



void __noopLog(const char*, ...);

}



#endif // LOGGING_H
