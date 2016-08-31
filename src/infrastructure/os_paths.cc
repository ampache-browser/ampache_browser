// os_paths.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifdef _WIN32
#include <Shlobj.h>
#include <locale>
#include <codecvt>
#include <sstream>
#else
#include <unistd.h>
#include <pwd.h>
#endif

#include <cstdlib>
#include <string>
#include "infrastructure/filesystem.h"
#include "infrastructure/os_paths.h"

using namespace std;



namespace infrastructure {

string OsPaths::getConfigHome() {
#ifdef _WIN32
    return getHome();
#else
    const char* configHome;
    if ((configHome = getenv("XDG_CONFIG_HOME")) && configHome[0] == PATH_SEP[0]) {
        return string{configHome} + PATH_SEP;
    }
    return getHome() + ".config" + PATH_SEP;
#endif
}



#ifndef _WIN32
string OsPaths::getCacheHome() {
    const char* cacheHome;
    if ((cacheHome = getenv("XDG_CACHE_HOME")) && cacheHome[0] == PATH_SEP[0]) {
        return string{cacheHome} + PATH_SEP;
    }
    return getHome() + ".cache" + PATH_SEP;
}
#endif



string OsPaths::getHome() {
#ifdef _WIN32
    TCHAR localAppData[MAX_PATH];
    SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, localAppData);
    wstringstream localAppDataWStream;
    localAppDataWStream << localAppData;
    return wstring_convert<codecvt_utf8<wchar_t>>().to_bytes(localAppDataWStream.str()) + PATH_SEP;
#else
    const char* home;
    if ((home = getenv("HOME")) && home[0] == PATH_SEP[0]) {
        return string{home} + PATH_SEP;
    }
    return string{getpwuid(getuid())->pw_dir} + PATH_SEP;
#endif
}

}
