// os_paths.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef OSPATHS_H
#define OSPATHS_H



#include <string>



namespace infrastructure {

/**
 * @brief Provides paths to standard disk locations.
 */
class OsPaths {

public:
    /**
     * @brief Path to user configuration directory.
     *
     * @return std::string
     */
    static std::string getConfigHome();

#ifndef _WIN32
    /**
     * @brief Path to uder cache directory.
     *
     * @return std::string
     */
    static std::string getCacheHome();
#endif

private:
    static std::string getHome();
};

}



#endif // OSPATHS_H
