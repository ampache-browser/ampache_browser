// request_groups.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <iostream>
#include "request_group.h"
#include "request_groups.h"

using namespace std;



namespace application {

RequestGroups::RequestGroups(int granularity):
myGranularity{granularity} { }



bool RequestGroups::isEmpty() const {
    return myRequestGroups.empty();
}



void RequestGroups::cutAndPlaceOnTop(RequestGroup requestGroup) {
    auto intersectingGroupIdxs = findIntersectingGroupIdxs(requestGroup);
    for (auto idx = intersectingGroupIdxs.size(); idx-- > 0;) {
        auto intersectingGroupIdx = intersectingGroupIdxs[idx];
        auto intersectingGroup = myRequestGroups[intersectingGroupIdx];
        pair<RequestGroup, RequestGroup> remainderGroups = intersectingGroup.substract(requestGroup);
        myRequestGroups.erase(myRequestGroups.begin() + intersectingGroupIdx);
        if (!remainderGroups.second.isEmpty()) {
            myRequestGroups.insert(myRequestGroups.begin() + intersectingGroupIdx, remainderGroups.second);
        }
        if (!remainderGroups.first.isEmpty()) {
            myRequestGroups.insert(myRequestGroups.begin() + intersectingGroupIdx, remainderGroups.first);
        }
    }
    myRequestGroups.push_back(requestGroup);
    chop();
}



bool RequestGroups::extend(int offset) {
    auto extensibleGroupIdx = findExtensibleGroupIdx(offset);
    if (extensibleGroupIdx == -1) {
        return false;
    }

    auto owningGroupIdx = findOwningGroupIdx(offset);

    auto& extensibleGroup = myRequestGroups[extensibleGroupIdx];
    extensibleGroup.extend();

    if (owningGroupIdx != -1) {
        auto& owningGroup = myRequestGroups[owningGroupIdx];
        owningGroup.shrink();
        if (owningGroup.isEmpty()) {
            myRequestGroups.erase(myRequestGroups.begin() + owningGroupIdx);
        }
    }
    chop();
    return true;
}



RequestGroup RequestGroups::pop() {
    auto poppedGroup = myRequestGroups.back();
    myRequestGroups.pop_back();
    return poppedGroup;
}



void RequestGroups::chop() {
    vector<RequestGroup> choppedGroups;
    for (auto& group: myRequestGroups) {
        if (group.getSize() > myGranularity) {
            auto split = group.split(group.getUpper() - (myGranularity - 1));
            while (split.first.getSize() > myGranularity) {
                choppedGroups.push_back(split.second);
                split = group.split(split.first.getUpper() - (myGranularity - 1));
            }
            choppedGroups.push_back(split.second);
            choppedGroups.push_back(split.first);
        } else {
            appendOnTop(choppedGroups, group);
        }
    }
    myRequestGroups = choppedGroups;
}



void RequestGroups::appendOnTop(vector<RequestGroup>& groups, const RequestGroup groupToPlace) {
    if (groups.size() == 0) {
        groups.push_back(groupToPlace);
        return;
    }

    auto lastGroup = groups.back();

    RequestGroup jointGroup{};
    if (lastGroup.getUpper() + 1 == groupToPlace.getLower()) {
        jointGroup = RequestGroup{lastGroup.getLower(), groupToPlace.getUpper()};
    } else if (lastGroup.getLower() - 1 == groupToPlace.getUpper()) {
        jointGroup = RequestGroup{groupToPlace.getLower(), lastGroup.getUpper()};
    }
    if (!jointGroup.isEmpty()) {
        groups.pop_back();
        if (jointGroup.getSize() > myGranularity) {
            auto split = jointGroup.split(jointGroup.getLower() + myGranularity);
            groups.push_back(split.second);
            groups.push_back(split.first);
        } else {
            groups.push_back(jointGroup);
        }
    } else {
        groups.push_back(groupToPlace);
    }
}



int RequestGroups::findOwningGroupIdx(int offset) const {
    int idx = 0;
    for (auto& requestGroup: myRequestGroups) {
        if (requestGroup.isMember(offset)) {
            return idx;
        }
        idx++;
    }
    return -1;
}



int RequestGroups::findExtensibleGroupIdx(int offset) const {
    int idx = 0;
    for (auto requestGroup: myRequestGroups) {
        if (requestGroup.canBeExtendedToOffset(offset)) {
            return idx;
        }
        idx++;
    }
    return -1;
}



vector<int> RequestGroups::findIntersectingGroupIdxs(RequestGroup requestGroup) const {
    vector<int> intersectingGroups{};
    int idx = 0;
    for (auto& group: myRequestGroups) {
        if (requestGroup.hasIntersection(group)) {
            intersectingGroups.push_back(idx);
        }
        idx++;
    }
    return intersectingGroups;
}

}
