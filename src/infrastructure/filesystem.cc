// filesystem.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <dirent.h>
#endif

#include <sys/stat.h>
#include <string>
#include "infrastructure/filesystem.h"

using namespace std;



namespace infrastructure {

// mode is not used on Windows
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool Filesystem::makePath(const string& path, unsigned int mode) {
#pragma GCC diagnostic pop
    auto mkdirResult = MKDIR(path.c_str(), mode);
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
    return dirStat.st_mode & S_IFDIR;
}



bool Filesystem::removeAllFiles(const string& path) {
#ifdef _WIN32
    WIN32_FIND_DATA findData;
    auto findHandle = FindFirstFile((path + "*").c_str(), &findData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    do {
        string fileName = findData.cFileName;
        if (fileName != "." && fileName != "..") {
            auto filePath = path + fileName;
            SetFileAttributes(filePath.c_str(), FILE_ATTRIBUTE_NORMAL);
            DeleteFile(filePath.c_str());
        }
    } while (FindNextFile(findHandle, &findData) == TRUE);
    FindClose(findHandle);
#else
    auto dir = opendir(path.c_str());
    if (dir == nullptr) {
        return false;
    }

    dirent* file;
    while ((file = readdir(dir)) != nullptr) {
        string fileName{file->d_name};
        if (fileName != "." && fileName != "..")
        {
            remove((path + fileName).c_str());
        }
    }
    closedir(dir);
#endif

    return true;
}

}
