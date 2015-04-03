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
    myAlbumRequests->readyToExecute += bind(&ManagedAmpacheService::onReadyToExecuteAlbums, this, _1);
    myAmpacheService.readyAlbums += bind(&ManagedAmpacheService::onReadyAlbums, this, _1);
}



ManagedAmpacheService::~ManagedAmpacheService() {
    delete(myAlbumRequests);
}



void ManagedAmpacheService::requestAlbum(int offset) {
    myAlbumRequests->add(offset);
}



void ManagedAmpacheService::onReadyToExecuteAlbums(RequestGroup& requestGroup) {
    cout << "request: " << requestGroup.getLower() << ", " << requestGroup.getSize() << endl;
    myAmpacheService.requestAlbums(requestGroup.getLower(), requestGroup.getSize());
}



void ManagedAmpacheService::onReadyAlbums(ReadyAlbumsEventArgs& _readyAlbumsEventArgs) {
    QCoreApplication::processEvents();
    auto finishedRequestGroup = myAlbumRequests->setFinished();
    auto readyAlbumsEventArgs = ReadyAlbumsEventArgs{_readyAlbumsEventArgs.albums,
        finishedRequestGroup.getLower(), finishedRequestGroup.getSize()};
    progressAlbums(readyAlbumsEventArgs);
}

}
