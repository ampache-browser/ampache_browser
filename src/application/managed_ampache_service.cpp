// managed_ampache_service.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <iostream>
#include <QtCore/QCoreApplication>
#include "request_group.h"
#include "managed_ampache_service.h"

using namespace std;
using namespace placeholders;



namespace application {

ManagedAmpacheService::ManagedAmpacheService(AmpacheService& ampacheService):
myAmpacheService(ampacheService) {
    myAmpacheService.readyAlbums += bind(&ManagedAmpacheService::onReadyAlbums, this, _1);
}



ManagedAmpacheService::~ManagedAmpacheService() {
}



// TODO: myAlbumRequestGroups can be transformed into a class (something like already existing RequestTracker) which
// could execute algorithm below and fire events when the request should be executed.  This would be the separation
// of the algorithm for "dynamic" loading.  If it simplifies ManagedAmpacheService too much, it could be implemented
// directly in AmpacheService.
void ManagedAmpacheService::requestAlbum(int offset) {
    if (myCurrentRequestGroup.isEmpty()) {
        myCurrentRequestGroup = RequestGroup{offset, offset};
        cout << "request1: " << offset << endl;
        myAmpacheService.requestAlbums(offset,  1);
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
            auto& extensibleGroup = myAlbumRequestGroups[extensibleGroupIdx];
            cout << "grp extend: " << extensibleGroup.getLower() << ", " << extensibleGroup.getUpper() << endl;
            extensibleGroup.extend();
            if (extensibleGroupIdx < myAlbumRequestGroups.size() - 1) {
                cout << "grp move: " << extensibleGroup.getLower() << ", " << extensibleGroup.getUpper() << endl;
                auto groupCopy = extensibleGroup;
                myAlbumRequestGroups.erase(myAlbumRequestGroups.begin() + extensibleGroupIdx);
                myAlbumRequestGroups.push_back(groupCopy);
            }
        } else {
            cout << "grp new: " << offset << endl;
            myAlbumRequestGroups.push_back(RequestGroup{offset, offset});
        }
        return;
    }

    auto group = myAlbumRequestGroups[groupIdx];
    if (group.getSize() <= 3) {
        // "move" group to back
        if (groupIdx < myAlbumRequestGroups.size() - 1) {
            cout << "grp move: " << group.getLower() << ", " << group.getUpper() << endl;
            myAlbumRequestGroups.erase(myAlbumRequestGroups.begin() + groupIdx);
            myAlbumRequestGroups.push_back(group);
        }
    } else {
        // split group, create two small groups from the split and move them back
        pair<RequestGroup, RequestGroup> split = group.split(offset);
        RequestGroup lowerGroup = split.first.upperTake(3);
        RequestGroup upperGroup = split.second.lowerTake(3);

        myAlbumRequestGroups.erase(myAlbumRequestGroups.begin() + groupIdx);
        if (!split.first.isEmpty()) {
            cout << "grp split1: " << split.first.getLower() << ", " << split.first.getUpper() << endl;
            myAlbumRequestGroups.insert(myAlbumRequestGroups.begin() + groupIdx, split.first);
        }
        if (!split.second.isEmpty()) {
            cout << "grp split2: " << split.second.getLower() << ", " << split.second.getUpper() << endl;
            myAlbumRequestGroups.insert(myAlbumRequestGroups.begin() + groupIdx, split.second);
        }
        cout << "grp take upper: " << upperGroup.getLower() << ", " << upperGroup.getUpper() << endl;
        myAlbumRequestGroups.push_back(upperGroup);
        if (!lowerGroup.isEmpty()) {
            cout << "grp take lower: " << lowerGroup.getLower() << ", " << lowerGroup.getUpper() << endl;
            myAlbumRequestGroups.push_back(lowerGroup);
        }
    }
}



void ManagedAmpacheService::onReadyAlbums(ReadyAlbumsEventArgs& _readyAlbumsEventArgs) {
    QCoreApplication::processEvents();
    auto readyAlbumsEventArgs = ReadyAlbumsEventArgs{_readyAlbumsEventArgs.albums,
        myCurrentRequestGroup.getLower(), myCurrentRequestGroup.getSize()};

    if (!myAlbumRequestGroups.empty()) {
        myCurrentRequestGroup = myAlbumRequestGroups.back();
        myAlbumRequestGroups.pop_back();
        cout << "request2: " << myCurrentRequestGroup.getLower() << ", " << myCurrentRequestGroup.getSize() << endl;
        myAmpacheService.requestAlbums(myCurrentRequestGroup.getLower(), myCurrentRequestGroup.getSize());
    }
    else {
        myCurrentRequestGroup = RequestGroup{};
    }

    progressAlbums(readyAlbumsEventArgs);
}



int ManagedAmpacheService::findOwningGroupIdx(int offset) {
    int idx = 0;
    for (auto requestGroup: myAlbumRequestGroups) {
        if (requestGroup.isMember(offset)) {
            return idx;
        }
        idx++;
    }
    return -1;
}



int ManagedAmpacheService::findExtensibleGroupIdx(int offset) {
    int idx = 0;
    for (auto requestGroup: myAlbumRequestGroups) {
        if (requestGroup.canBeExtendedToOffset(offset)) {
            return idx;
        }
        idx++;
    }
    return -1;
}


}
