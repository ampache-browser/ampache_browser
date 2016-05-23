// qt_logger.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



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



LogLevel QtLogger::myMaxLogLevel = LogLevel::Info;



SetQtLoggerDefaultMessagePattern sqldmp{};

}
