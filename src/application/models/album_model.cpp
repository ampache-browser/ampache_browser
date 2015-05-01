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
#include <QtGui/QIcon>

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
    myArtRequests->readyToExecute += bind(&AlbumModel::onReadyToExecuteArts, this, _1);
    myAmpacheService.readyAlbumArts += bind(&AlbumModel::onReadyAlbumArts, this, _1);
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
    } else if (role == Qt::DecorationRole) {
        // SMELL: size specified on multiple places
        QPixmap notLoadedPixmap{100, 100};
        notLoadedPixmap.fill(Qt::GlobalColor::lightGray);
        notLoaded = QIcon{notLoadedPixmap};
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
        return QVariant{QString::fromStdString(indexAndAlbum->second.second->getName())};
    } else if (role == Qt::DecorationRole) {
        if (indexAndAlbum->second.second->getArt() == nullptr) {
            myArtRequests->add(row);
            return notLoaded;
        } else {
            return QIcon{*(indexAndAlbum->second.second->getArt())};
        }
    } else {
        return QVariant{};
    }
}



int AlbumModel::rowCount(const QModelIndex&) const {
    return myAmpacheService.numberOfAlbums();
}



void AlbumModel::onReadyToExecuteAlbums(RequestGroup& requestGroup) {
    myAmpacheService.requestAlbums(requestGroup.getLower(), requestGroup.getSize());
}



void AlbumModel::onReadyAlbums(vector<pair<string, unique_ptr<Album>>>& artUrlsAndAlbums) {
    auto finishedRequestGroup = myAlbumRequests->setFinished();

    int row = finishedRequestGroup.getLower();
    for (auto& urlAndAlbum: artUrlsAndAlbums) {
        myAlbums[row++] = move(urlAndAlbum);
    }

    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}



void AlbumModel::onReadyToExecuteArts(RequestGroup& requestGroup) {
    vector<string> urls;
    for (auto idx = requestGroup.getLower(); idx <= requestGroup.getUpper(); idx++) {
        auto indexAndAlbum = myAlbums.find(idx);
        urls.push_back(indexAndAlbum->second.first);
    }
    myAmpacheService.requestAlbumArts(urls);
}



void AlbumModel::onReadyAlbumArts(map<string, QPixmap>& artUrlsAndArts) {
    auto finishedRequestGroup = myArtRequests->setFinished();

    for (auto artUrlAndArt: artUrlsAndArts) {
        for (auto& album: myAlbums) {
            if (album.second.first == artUrlAndArt.first) {
                album.second.second->setArt(new QPixmap{artUrlAndArt.second});
                break;
            }
        }
    }

    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}

}
