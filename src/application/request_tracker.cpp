// request_tracker.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "request_tracker.h"



namespace application {

bool RequestTracker::isRequested(int row) const {
    if (myRequests.find(row) == myRequests.end()) {
        return false;
    } else {
        return true;
    }
}



void RequestTracker::addRequests(int row, int count) {
    for (int r = row; r < row + count; r++) {
        myRequests.insert(r);
    }
}



void RequestTracker::removeRequests(int row, int count) {
    for (int r = row; r < row + count; r++) {
        myRequests.erase(r);
    }
}

}
