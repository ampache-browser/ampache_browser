// album_model.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <iostream>
#include <vector>
#include <memory>

#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractListModel>

#include "domain/album.h"
#include "ampache_service.h"
#include "application/album_model.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace application {

AlbumModel::AlbumModel(AmpacheService& ampacheService, QObject* parent): QAbstractListModel(parent),
myAmpacheService(ampacheService) {
    myManagedAmpacheService = new ManagedAmpacheService{myAmpacheService};
    myManagedAmpacheService->progressAlbums += bind(&AlbumModel::onProgressAlbums, this, _1);
}



AlbumModel::~AlbumModel() {
    delete(myManagedAmpacheService);
}



QVariant AlbumModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant{};
    }

    QVariant notLoaded{};
    if (role == Qt::DisplayRole) {
        notLoaded = QVariant{QString{"loading..."}};
    }

    int row = index.row();
    auto indexAndAlbum = myAlbums.find(row);
    if (indexAndAlbum == myAlbums.end()) {
        if (role == Qt::DisplayRole) {
            myManagedAmpacheService->requestAlbum(row);
        }
        return notLoaded;
    }

    if (role == Qt::DisplayRole) {
        return QVariant{QString::fromStdString(indexAndAlbum->second->getName())};
    } else {
        return QVariant{};
    }
}



int AlbumModel::rowCount(const QModelIndex& parent) const {
    return myAmpacheService.numberOfAlbums();
}



void AlbumModel::onProgressAlbums(ReadyAlbumsEventArgs& readyAlbumsEventArgs) {
    int idx = readyAlbumsEventArgs.offset;
    for (auto& album: readyAlbumsEventArgs.albums) {
        myAlbums[idx++] = move(album);
    }

    dataChanged(createIndex(readyAlbumsEventArgs.offset, 0),
        createIndex(readyAlbumsEventArgs.offset + readyAlbumsEventArgs.limit - 1, 0));
}

}
