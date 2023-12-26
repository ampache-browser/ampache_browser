// requests.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2018 Róbert Čerňanský



#include "requests.h"

using namespace std;



namespace application {

Requests::Requests(int granularity):
myRequestGroups{new RequestGroups{granularity}} { }



Requests::Requests(): Requests(0) { }



// SMELL: The algorithm is not clearly visible from here because it heavily relies on RequestGroups behaviour (sorting
// chops backwards, etc.).
void Requests::add(int offset) {
    if (!isInProgress()) {
        myCurrentRequestGroup = RequestGroup{offset, offset};
        readyToExecute(myCurrentRequestGroup);
        return;
    }
    if (myCurrentRequestGroup.isMember(offset))
    {
        return;
    }

    if (offset < myLastEnqueuedOffset) {
        myRequestGroups->moveOnTop(RequestGroup{offset, offset});
    } else if (offset > myLastEnqueuedOffset + 1) {
        myRequestGroups->moveOnTop(RequestGroup{offset, offset});
    } else {
        myRequestGroups->extend(offset);
    }
    myLastEnqueuedOffset = offset;
}



void Requests::removeAll() {
    myRequestGroups->clear();
}



void Requests::setFinished(int offset, int count) {
    auto finishedRequestGroup = RequestGroup{offset, offset + count - 1};

    // finished requests may be different from what was ready to execution; if there were some requests finished which
    // were not ready remainderGroups will contain them; they have to be cut from myRequestGroups
    auto remainderGroups = finishedRequestGroup.substract(myCurrentRequestGroup);
    myRequestGroups->cut(remainderGroups.first);
    myRequestGroups->cut(remainderGroups.second);

    if (!myRequestGroups->isEmpty()) {
        myCurrentRequestGroup = myRequestGroups->pop();
        readyToExecute(myCurrentRequestGroup);
    } else {
        myCurrentRequestGroup = RequestGroup{};
        myLastEnqueuedOffset = numeric_limits<int>::max();
    }
}



bool Requests::isInProgress() const {
    return !myCurrentRequestGroup.isEmpty();
}

}
