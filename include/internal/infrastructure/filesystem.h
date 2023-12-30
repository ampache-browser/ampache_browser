// filesystem.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2023 Róbert Čerňanský



#ifndef FILESYSTEM_H
#define FILESYSTEM_H



#ifdef _WIN32
#include "infrastructure/string_encoding.h"
#else
#include <sys/stat.h>
#endif

#include <string>



namespace infrastructure {

#ifdef _WIN32
#define MKDIR(pathName, mode) _wmkdir(StringEncoding::utf8ToWide(pathName).c_str())
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
    /**
     * @brief Creates a directory specified by path, creating all parent directories if necessary.
     *
     * @param path Path consisting from directories that shall be created.
     * @param mode Mode that shall be used when creating directories (see 'stat' documentation). Ignored on Windows.
     * @return true if the directory was created, false otherwise (e. g. if it already existed or some error occured).
     */
    static bool makePath(const std::string& path, unsigned int mode = 0700);

    /**
     * @brief Checks whether the given directory exists.
     *
     * @param path Path to the tested directory.
     * @return true if the directory exists, false otherwise.
     */
    static bool isDirExisting(const std::string& path);

    /**
     * @brief Removes all files from the give directory. Does not remove nor traverse subdirectories.
     *
     * @param path Path to the directory to remove all files from.  Must end with PATH_SEP.
     * @return true if removal was successful, false otherwise.
     */
    static bool removeAllFiles(const std::string& path);
};

}



#endif // FILESYSTEM_H
