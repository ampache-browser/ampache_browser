// os_paths.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef OSPATHS_H
#define OSPATHS_H



#include <string>



namespace infrastructure {

class OsPaths {

public:
    static std::string getConfigHome();

    static std::string getCacheHome();

private:
    static std::string getHome();
};

}



#endif // OSPATHS_H
