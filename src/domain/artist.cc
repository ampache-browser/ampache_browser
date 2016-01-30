// artist.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <string>
#include <functional>
#include "domain/artist.h"

using namespace std;



namespace domain {

Artist::Artist(const string& id, const string& name):
myId{id},
myName{name} {
}



const string Artist::getId() const {
    return myId;
}



const string Artist::getName() const {
    return myName;
}



bool operator==(const Artist& lhs, const Artist& rhs) {
    return lhs.getId() == rhs.getId();
}



bool operator!=(const Artist& lhs, const Artist& rhs) {
    return !operator==(lhs, rhs);
}



bool operator<(const Artist& lhs, const Artist& rhs) {
    return (lhs.getId() != rhs.getId()) && (lhs.getName() < rhs.getName());
}



bool operator>(const Artist& lhs, const Artist& rhs) {
    return operator<(rhs, lhs);
}



bool operator<=(const Artist& lhs, const Artist& rhs) {
    return !operator>(lhs, rhs);
}



bool operator>=(const Artist& lhs, const Artist& rhs) {
    return !operator<(lhs, rhs);
}

}



namespace std {

size_t hash<domain::Artist>::operator()(const domain::Artist& artist) const {
    return hash<string>()(artist.getId());
}

}
