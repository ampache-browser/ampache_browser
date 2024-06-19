// qt_logger.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <QDebug>
#include <QMessageLogger>

#include "infrastructure/logging/log_level.h"
#include "set_qt_logger_default_message_pattern.h"
#include "infrastructure/logging/qt_logger/qt_logger.h"



namespace infrastructure {

QtLogger::QtLogger(const char* file, int line, const char* function):
myFile(file),
myLine(line),
myFunction(function) {
}



void QtLogger::setMaxLogLevel(LogLevel maxLogLevel) {
    myMaxLogLevel = maxLogLevel;
}



void QtLogger::error(const char* message) const {
    if (QtLogger::myMaxLogLevel >= LogLevel::Error) {
        QMessageLogger(myFile, myLine, myFunction).critical() << message;
    }
}



void QtLogger::warning(const char* message) const {
    if (QtLogger::myMaxLogLevel >= LogLevel::Warning) {
        QMessageLogger(myFile, myLine, myFunction).warning() << message;
    }
}



void QtLogger::info(const char* message) const {
    if (QtLogger::myMaxLogLevel >= LogLevel::Info) {
        QMessageLogger(myFile, myLine, myFunction).info() << message;
    }
}



void QtLogger::debug(const char* message) const {
    if (QtLogger::myMaxLogLevel >= LogLevel::Debug) {
        QMessageLogger(myFile, myLine, myFunction).debug() << message;
    }
}



LogLevel QtLogger::myMaxLogLevel = LogLevel::Info;



SetQtLoggerDefaultMessagePattern sqldmp{};

}
