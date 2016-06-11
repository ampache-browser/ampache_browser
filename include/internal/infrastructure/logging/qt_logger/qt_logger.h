// qt_logger.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef QTLOGGER_H
#define QTLOGGER_H



#include <QMessageLogger>
#include "infrastructure/logging/log_level.h"



namespace infrastructure {

/**
 * @brief Writes logging messages using Qt platfom logging.
 */
class QtLogger {

public:
    /**
     * @brief Creates instance.
     *
     * @param file File name where the log message origins from.
     * @param line Line number in the given file.
     * @param function Funcion or method name where the log message has occured.
     */
    explicit QtLogger(const char* file, int line, const char* function);

    /**
     * @brief Sets the maximal loggin level.
     *
     * If a log method with lower level than maxLogLevel is called, no message is logged.
     *
     * @param maxLogLevel Maximal logging level.
     *
     * @sa error(), warning(), info(), debug()
     */
    static void setMaxLogLevel(LogLevel maxLogLevel);

    /**
     * @brief Logs an error (LogLevel::Error) message.
     *
     * @param args Message as a format string and its arguments.
     */
    template <typename... TArgs>
    void error(TArgs... args) const;

    /**
     * @brief Logs an error (LogLevel::Error) message.
     */
    void error(const char* message) const;

    /**
     * @brief Logs a warning (LogLevel::Warning) message.
     *
     * @param args Message as a format string and its arguments.
     */
    template <typename... TArgs>
    void warning(TArgs... args) const;

    /**
     * @brief Logs a warning (LogLevel::Warning) message.
     */
    void warning(const char* message) const;

    /**
     * @brief Logs an info (LogLevel::Info) message.
     *
     * @param args Message as a format string and its arguments.
     */
    template <typename... TArgs>
    void info(TArgs... args) const;

    /**
     * @brief Logs an info (LogLevel::Info) message.
     */
    void info(const char* message) const;

    /**
     * @brief Logs a debug (LogLevel::Debug) message.
     *
     * @param args Message as a format string and its arguments.
     */
    template <typename... TArgs>
    void debug(TArgs... args) const;

    /**
     * @brief Logs a debug (LogLevel::Debug) message.
     */
    void debug(const char* message) const;

private:
    const char* myFile;
    int myLine;
    const char* myFunction;

    static LogLevel myMaxLogLevel;
};




template <typename... TArgs>
void QtLogger::error(TArgs... args) const {
    if (QtLogger::myMaxLogLevel >= LogLevel::Error) {
        QMessageLogger(myFile, myLine, myFunction).critical(args...);
    }
}



template <typename... TArgs>
void QtLogger::warning(TArgs... args) const {
    if (QtLogger::myMaxLogLevel >= LogLevel::Warning) {
        QMessageLogger(myFile, myLine, myFunction).warning(args...);
    }
}



template <typename... TArgs>
void QtLogger::info(TArgs... args) const {
    if (QtLogger::myMaxLogLevel >= LogLevel::Info) {
        QMessageLogger(myFile, myLine, myFunction).info(args...);
    }
}



template <typename... TArgs>
void QtLogger::debug(TArgs... args) const {
    if (QtLogger::myMaxLogLevel >= LogLevel::Debug) {
        QMessageLogger(myFile, myLine, myFunction).debug(args...);
    }
}

}



#endif // QTLOGGER_H
