// log_level.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef LOGLEVEL_H
#define LOGLEVEL_H



namespace infrastructure {

enum class LogLevel {
    None = 0, 
    Error = 10,
    Warning = 20,
    Info = 30,
    Debug = 40
};

}



#endif // LOGLEVEL_H

