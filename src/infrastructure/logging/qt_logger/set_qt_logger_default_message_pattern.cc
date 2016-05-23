
#include <QString>
#include <QtGlobal>
#include "set_qt_logger_default_message_pattern.h"



SetQtLoggerDefaultMessagePattern::SetQtLoggerDefaultMessagePattern() {
    qSetMessagePattern(QString{"%{type} %{file}:%{line} [%{function}]: %{message}"});
}
