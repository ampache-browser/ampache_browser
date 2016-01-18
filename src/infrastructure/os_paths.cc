// os_paths.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <unistd.h>
#include <pwd.h>
#include <cstdlib>
#include <string>
#include "infrastructure/os_paths.h"

using namespace std;



namespace infrastructure {

string OsPaths::getConfigHome() {
    const char* configHome;
    if ((configHome = getenv("XDG_CONFIG_HOME")) && configHome[0] == '/') {
        return string{configHome} + "/";
    }
    return getHome() + "/.config/";
}



string OsPaths::getCacheHome() {
    const char* cacheHome;
    if ((cacheHome = getenv("XDG_CACHE_HOME")) && cacheHome[0] == '/') {
        return string{cacheHome} + "/";
    }
    return getHome() + "/.cache/";
}



string OsPaths::getHome() {
    const char* home;
    if ((home = getenv("HOME"))) {
        home = getpwuid(getuid())->pw_dir;
    }
    return home;
}

}
