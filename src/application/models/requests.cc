// requests.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <iostream>
#include "requests.h"

using namespace std;



namespace application {

Requests::Requests(int granularity):
myRequestGroups{new RequestGroups{granularity}} { }



Requests::Requests(): Requests(0) { }



// SMELL: The algorithm is not clearly visible from here because it heavilly relies on RequestGroups behaviour (sorting
// chops backwards, etc.).
void Requests::add(int offset) {
    if (myCurrentRequestGroup.isEmpty()) {
        myCurrentRequestGroup = RequestGroup{offset, offset};
        readyToExecute(myCurrentRequestGroup);
        return;
    }
    if (myCurrentRequestGroup.isMember(offset))
    {
        return;
    }

    if (offset < myLastEnqueuedOffset) {
        myRequestGroups->cutAndPlaceOnTop(RequestGroup{offset, offset});
    } else if (offset > myLastEnqueuedOffset + 1) {
        myRequestGroups->cutAndPlaceOnTop(RequestGroup{offset, offset});
    } else {
        myRequestGroups->extend(offset);
    }
    myLastEnqueuedOffset = offset;
}



void Requests::cancel() {
    myRequestGroups->clear();
}



// SMELL: Not necesary to expose RequestGroup.  Return just pair and Requests will be the only
// "interface" class to request handling.  Same for readyToExecute event.
RequestGroup Requests::setFinished() {
    auto finishedRequestGroup = myCurrentRequestGroup;
    if (!myRequestGroups->isEmpty()) {
        myCurrentRequestGroup = myRequestGroups->pop();
        readyToExecute(myCurrentRequestGroup);
    } else {
        myCurrentRequestGroup = RequestGroup{};
        myLastEnqueuedOffset = numeric_limits<int>::max();
    }
    return finishedRequestGroup;
}

}
