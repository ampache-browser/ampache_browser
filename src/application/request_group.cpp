// request_group.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



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



void RequestGroup::extend() {
    if (isEmpty()) {
        myLower = myUpper + 1;
    }
    myUpper++;
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



RequestGroup RequestGroup::upperTake(int size) {
    if (isEmpty() || size <= 0) {
        return RequestGroup{};
    }

    if (size > getSize()) {
        size = getSize();
    }

    auto takenPart = RequestGroup{myUpper - (size - 1), myUpper};
    myUpper -= size;
    return takenPart;
}



RequestGroup RequestGroup::lowerTake(int size) {
    if (isEmpty() || size <= 0) {
        return RequestGroup{};
    }

    if (size > getSize()) {
        size = getSize();
    }

    auto takenPart = RequestGroup{myLower, myLower + (size - 1)};
    myLower += size;
    return takenPart;
}

}
