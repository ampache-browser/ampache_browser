// managed_ampache_service.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef MANAGEDAMPACHESERVICE_H
#define MANAGEDAMPACHESERVICE_H



#include <unordered_set>
#include <vector>
#include "infrastructure/event.h"
#include "request_group.h"
#include "ampache_service.h"



namespace application {

class ManagedAmpacheService {

public:
    explicit ManagedAmpacheService(AmpacheService& ampacheService);

    ~ManagedAmpacheService();

    Event<ReadyAlbumsEventArgs> progressAlbums{};

    void requestAlbum(int offset);

private:
    AmpacheService& myAmpacheService;
    unordered_set<int> myAlbumRequests;
    vector<RequestGroup> myAlbumRequestGroups;
    RequestGroup myCurrentRequestGroup = RequestGroup{};

    void onReadyAlbums(ReadyAlbumsEventArgs& readyAlbumsEventArgs);
    bool isAlbumRequested(int offset);
    int findOwningGroupIdx(int offset);
    int findExtensibleGroupIdx(int offset);
};

}



#endif // MANAGEDAMPACHESERVICE_H
