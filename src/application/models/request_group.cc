// request_group.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <algorithm>
#include "request_group.h"

using namespace std;



namespace application {

RequestGroup::RequestGroup(int lower, int upper):
myLower{lower},
myUpper{upper} { }



RequestGroup::RequestGroup(): RequestGroup{-1, -1} { }



bool RequestGroup::isEmpty() const {
    if (myLower < 0 || myUpper < 0 || myUpper < myLower) {
        return true;
    }
    return false;
}



int RequestGroup::getLower() const {
    return myLower;
}



int RequestGroup::getUpper() const {
    return myUpper;
}



int RequestGroup::getSize() const {
    if (isEmpty()) {
        return 0;
    }
    return myUpper - myLower + 1;
}



bool RequestGroup::isMember(int offset) const {
    if (!isEmpty() && offset >= 0 && offset >= myLower && offset <= myUpper) {
        return true;
    }
    return false;
}



bool RequestGroup::canBeExtendedToOffset(int offset) const {
    if (offset >= 0 && offset == myUpper + 1) {
        return true;
    }
    return false;
}



bool RequestGroup::hasIntersection(RequestGroup other) const {
    if (myUpper < other.getLower() || myLower > other.getUpper()) {
        return false;
    }
    return true;
}



pair<RequestGroup, RequestGroup> RequestGroup::split(int offset) const {
    if (!isMember(offset)) {
        return pair<RequestGroup, RequestGroup>{RequestGroup{}, RequestGroup{}};
    }

    RequestGroup lowerPart;
    if (offset == myLower) {
        lowerPart = RequestGroup{};
    } else {
        lowerPart = RequestGroup{myLower, offset - 1};
    }
    auto upperPart = RequestGroup{offset, myUpper};
    return pair<RequestGroup, RequestGroup>{lowerPart, upperPart};
}



pair<RequestGroup, RequestGroup> RequestGroup::substract(RequestGroup other) const {
    if (!hasIntersection(other)) {
        return pair<RequestGroup, RequestGroup>{RequestGroup{}, RequestGroup{}};
    }
    return pair<RequestGroup, RequestGroup>{RequestGroup{myLower, other.getLower() - 1},
        RequestGroup{other.getUpper() + 1, myUpper}};
}



void RequestGroup::extend() {
    if (isEmpty()) {
        myLower = myUpper + 1;
    }
    myUpper++;
}



void RequestGroup::shrink() {
    myLower++;
}

}
