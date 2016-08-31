// filesystem.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef FILESYSTEM_H
#define FILESYSTEM_H



#include <sys/stat.h>
#include <string>



namespace infrastructure {

#ifdef _WIN32
#define MKDIR(pathName, mode) mkdir(pathName)
#define PATH_SEP "\\"
#else
#define MKDIR(pathName, mode) mkdir(pathName, mode)
#define PATH_SEP "/"
#endif



/**
 * @brief Operations on a filesystem with files and directories.
 */
class Filesystem {

public:
    static bool makePath(const std::string& path, mode_t mode = 0700);

    static bool isDirExisting(const std::string& path);
};

}



#endif // FILESYSTEM_H
