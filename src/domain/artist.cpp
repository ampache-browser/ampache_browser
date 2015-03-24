// artist.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <string>
#include "domain/artist.h"

using namespace std;



namespace domain {

Artist::Artist(const string &name):
myName{name} {
}



Artist::~Artist() {
}



const string Artist::getName() const {
    return myName;
}

}
