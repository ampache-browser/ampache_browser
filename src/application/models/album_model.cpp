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
#include "../ampache_service.h"
#include "requests.h"
#include "application/models/album_model.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace application {

AlbumModel::AlbumModel(AmpacheService& ampacheService, QObject* parent): QAbstractListModel(parent),
myAmpacheService(ampacheService) {
    myAlbumRequests->readyToExecute += bind(&AlbumModel::onReadyToExecuteAlbums, this, _1);
    myAmpacheService.readyAlbums += bind(&AlbumModel::onReadyAlbums, this, _1);
}



AlbumModel::~AlbumModel() {
    delete(myAlbumRequests);
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
            myAlbumRequests->add(row);
        }
        return notLoaded;
    }

    if (role == Qt::DisplayRole) {
        return QVariant{QString::fromStdString(indexAndAlbum->second->getName())};
    } else {
        return QVariant{};
    }
}



int AlbumModel::rowCount(const QModelIndex&) const {
    return myAmpacheService.numberOfAlbums();
}



void AlbumModel::onReadyToExecuteAlbums(application::RequestGroup& requestGroup) {
    myAmpacheService.requestAlbums(requestGroup.getLower(), requestGroup.getSize());
}



void AlbumModel::onReadyAlbums(vector<unique_ptr<Album>>& albums) {
    auto finishedRequestGroup = myAlbumRequests->setFinished();

    int row = finishedRequestGroup.getLower();
    for (auto& album: albums) {
        myAlbums[row++] = move(album);
    }

    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}

}
