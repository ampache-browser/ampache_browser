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

Artist::Artist(const string id, const string name):
myId{id},
myName{name} {
}



Artist::~Artist() {
}



const string Artist::getId() const {
    return myId;
}



const string Artist::getName() const {
    return myName;
}

}
