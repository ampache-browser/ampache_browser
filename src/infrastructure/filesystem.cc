// filesystem.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <sys/stat.h>
#include <string>
#include "infrastructure/filesystem.h"

using namespace std;



namespace infrastructure {

// mode is not used on Windows
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool Filesystem::makePath(const string& path, mode_t mode) {
#pragma GCC diagnostic pop
    auto mkdirResult = mkdir(path.c_str(), mode);
    if (mkdirResult == 0) {
        return true;
    }

    if (errno == ENOENT) {
        if (makePath(path.substr(0, path.find_last_of(PATH_SEP)))) {
            return MKDIR(path.c_str(), mode) == 0;
        }
    }

    return false;
}



bool Filesystem::isDirExisting(const string& path) {
    struct stat dirStat;
    stat(path.c_str(), &dirStat);
    return S_ISDIR(dirStat.st_mode);
}

}
