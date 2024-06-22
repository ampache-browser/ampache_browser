// filesystem.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <cstdio>
#endif

#include <sys/stat.h>
#include <cerrno>
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
#ifdef _WIN32
    struct _stat dirStat;
    _wstat(StringEncoding::utf8ToWide(path).c_str(), &dirStat);
#else
    struct stat dirStat;
    stat(path.c_str(), &dirStat);
#endif
    return dirStat.st_mode & S_IFDIR;

}



bool Filesystem::removeAllFiles(const string& path) {
#ifdef _WIN32
    wstring pathW = StringEncoding::utf8ToWide(path);

    WIN32_FIND_DATAW findData;
    HANDLE findHandle = FindFirstFileW((pathW + L"*").c_str(), &findData);
    if (findHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    do {
        wstring fileName = findData.cFileName;
        if (fileName != L"." && fileName != L"..") {
            wstring filePath = pathW + fileName;
            SetFileAttributesW(filePath.c_str(), FILE_ATTRIBUTE_NORMAL);
            DeleteFileW(filePath.c_str());
        }
    } while (FindNextFileW(findHandle, &findData) == TRUE);
    FindClose(findHandle);
#else
    auto dir = opendir(path.c_str());
    if (dir == nullptr) {
        return false;
    }

    dirent* file;
    while ((file = readdir(dir)) != nullptr) {
        string fileName{file->d_name};
        if (fileName != "." && fileName != "..") {
            remove((path + fileName).c_str());
        }
    }
    closedir(dir);
#endif

    return true;
}

}
