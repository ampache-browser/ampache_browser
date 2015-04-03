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
#include "requests.h"



namespace application {

class ManagedAmpacheService {

public:
    explicit ManagedAmpacheService(AmpacheService& ampacheService);

    ~ManagedAmpacheService();

    Event<ReadyAlbumsEventArgs> progressAlbums{};

    void requestAlbum(int offset);

private:
    AmpacheService& myAmpacheService;
    Requests* myAlbumRequests = new Requests{};

    void onReadyToExecuteAlbums(RequestGroup& requestGroup);

    void onReadyAlbums(ReadyAlbumsEventArgs& readyAlbumsEventArgs);
};

}



#endif // MANAGEDAMPACHESERVICE_H
