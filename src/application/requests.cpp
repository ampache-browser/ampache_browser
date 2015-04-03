// requests.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <iostream>
#include "requests.h"

using namespace std;



namespace application {

void Requests::add(int offset) {
    if (myCurrentRequestGroup.isEmpty()) {
        myCurrentRequestGroup = RequestGroup{offset, offset};
//         cout << "request1: " << offset << endl;
        readyToExecute(myCurrentRequestGroup);
        return;
    }

    if (myCurrentRequestGroup.isMember(offset))
    {
        return;
    }

    auto groupIdx = findOwningGroupIdx(offset);
    if (groupIdx == -1) {
        auto extensibleGroupIdx = findExtensibleGroupIdx(offset);
        if (extensibleGroupIdx != -1) {
            auto& extensibleGroup = myRequestGroups[extensibleGroupIdx];
//             cout << "grp extend: " << extensibleGroup.getLower() << ", " << extensibleGroup.getUpper() << endl;
            extensibleGroup.extend();
            if (extensibleGroupIdx < myRequestGroups.size() - 1) {
//                 cout << "grp move: " << extensibleGroup.getLower() << ", " << extensibleGroup.getUpper() << endl;
                auto groupCopy = extensibleGroup;
                myRequestGroups.erase(myRequestGroups.begin() + extensibleGroupIdx);
                myRequestGroups.push_back(groupCopy);
            }
        } else {
//             cout << "grp new: " << offset << endl;
            myRequestGroups.push_back(RequestGroup{offset, offset});
        }
        return;
    }

    auto group = myRequestGroups[groupIdx];
    if (group.getSize() <= 3) {
        // "move" group to back
        if (groupIdx < myRequestGroups.size() - 1) {
//             cout << "grp move: " << group.getLower() << ", " << group.getUpper() << endl;
            myRequestGroups.erase(myRequestGroups.begin() + groupIdx);
            myRequestGroups.push_back(group);
        }
    } else {
        // split group, create two small groups from the split and move them back
        pair<RequestGroup, RequestGroup> split = group.split(offset);
        RequestGroup lowerGroup = split.first.upperTake(3);
        RequestGroup upperGroup = split.second.lowerTake(3);

        myRequestGroups.erase(myRequestGroups.begin() + groupIdx);
        if (!split.first.isEmpty()) {
//             cout << "grp split1: " << split.first.getLower() << ", " << split.first.getUpper() << endl;
            myRequestGroups.insert(myRequestGroups.begin() + groupIdx, split.first);
        }
        if (!split.second.isEmpty()) {
//             cout << "grp split2: " << split.second.getLower() << ", " << split.second.getUpper() << endl;
            myRequestGroups.insert(myRequestGroups.begin() + groupIdx, split.second);
        }
//         cout << "grp take upper: " << upperGroup.getLower() << ", " << upperGroup.getUpper() << endl;
        myRequestGroups.push_back(upperGroup);
        if (!lowerGroup.isEmpty()) {
//             cout << "grp take lower: " << lowerGroup.getLower() << ", " << lowerGroup.getUpper() << endl;
            myRequestGroups.push_back(lowerGroup);
        }
    }
}



RequestGroup Requests::setFinished() {
    auto finishedRequestGroup = myCurrentRequestGroup;
    if (!myRequestGroups.empty()) {
        myCurrentRequestGroup = myRequestGroups.back();
        myRequestGroups.pop_back();
//         cout << "request2: " << myCurrentRequestGroup.getLower() << ", " << myCurrentRequestGroup.getSize() << endl;
        readyToExecute(myCurrentRequestGroup);
    }
    else {
        myCurrentRequestGroup = RequestGroup{};
    }
    return finishedRequestGroup;
}



int Requests::findOwningGroupIdx(int offset) {
    int idx = 0;
    for (auto requestGroup: myRequestGroups) {
        if (requestGroup.isMember(offset)) {
            return idx;
        }
        idx++;
    }
    return -1;
}



int Requests::findExtensibleGroupIdx(int offset) {
    int idx = 0;
    for (auto requestGroup: myRequestGroups) {
        if (requestGroup.canBeExtendedToOffset(offset)) {
            return idx;
        }
        idx++;
    }
    return -1;
}

}
