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
#include "data/album_repository.h"
#include "requests.h"
#include "application/models/album_model.h"

using namespace std;
using namespace placeholders;
using namespace data;
using namespace domain;



namespace application {

AlbumModel::AlbumModel(AlbumRepository& albumRepository, QObject* parent): QAbstractListModel(parent),
myAlbumRepository(albumRepository) {
    myAlbumRequests->readyToExecute += bind(&AlbumModel::onReadyToExecuteAlbums, this, _1);
    myAlbumRepository.loaded += bind(&AlbumModel::onLoaded, this, _1);
    myArtRequests->readyToExecute += bind(&AlbumModel::onReadyToExecuteArts, this, _1);
    myAlbumRepository.artsLoaded += bind(&AlbumModel::onArtsLoaded, this, _1);

    // start populating with data
    for (int row; row < rowCount(); row++) {
        myAlbumRequests->add(row);
    }
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
    if (!myAlbumRepository.isLoaded(row)) {
        if (role == Qt::DisplayRole) {
            myAlbumRequests->add(row);
        }
        return notLoaded;
    }

    auto& album = myAlbumRepository.get(row);
    if (role == Qt::DisplayRole) {
        return QString::fromStdString(album.getName());
    } else if (role == Qt::DecorationRole) {
        if (album.getArt() == nullptr) {
            myArtRequests->add(row);
            return notLoaded;
        } else {
            return QIcon{*(album.getArt())};
        }
    } else {
        return QVariant{};
    }
}



int AlbumModel::rowCount(const QModelIndex&) const {
    return myAlbumRepository.maxCount();
}



void AlbumModel::onReadyToExecuteAlbums(RequestGroup& requestGroup) {
    myAlbumRepository.load(requestGroup.getLower(), requestGroup.getSize());
}



void AlbumModel::onLoaded(pair<int, int>&) {
    auto finishedRequestGroup = myAlbumRequests->setFinished();
    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}



void AlbumModel::onReadyToExecuteArts(RequestGroup& requestGroup) {
    myAlbumRepository.loadArts(requestGroup.getLower(), requestGroup.getSize());
}



void AlbumModel::onArtsLoaded(pair<int, int>&) {
    auto finishedRequestGroup = myArtRequests->setFinished();
    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}

}
