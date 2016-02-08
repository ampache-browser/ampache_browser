// album_model.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <vector>
#include <memory>

#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractListModel>
#include <QtGui/QIcon>

#include "infrastructure/event/delegate.h"
#include "domain/album.h"
#include "data/album_repository.h"
#include "requests.h"
#include "application/models/album_model.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace data;
using namespace domain;



namespace application {

AlbumModel::AlbumModel(AlbumRepository& albumRepository, QObject* parent): QAbstractTableModel(parent),
myAlbumRepository(albumRepository) {
    myAlbumRequests->readyToExecute += DELEGATE1(&AlbumModel::onReadyToExecuteAlbums, RequestGroup);
    myAlbumRepository.loaded += DELEGATE1(&AlbumModel::onLoaded, pair<int, int>);
    myArtRequests->readyToExecute += DELEGATE1(&AlbumModel::onReadyToExecuteArts, RequestGroup);
    myAlbumRepository.artsLoaded += DELEGATE1(&AlbumModel::onArtsLoaded, pair<int, int>);
    myAlbumRepository.filterChanged += DELEGATE0(&AlbumModel::onFilterChanged);
}



QVariant AlbumModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::DecorationRole) || myDataRequestsAborted) {
        return QVariant{};
    }

    QVariant notLoaded{};
    if (role == Qt::DisplayRole) {
        notLoaded = QVariant{QString{"loading..."}};
    } else {
        // SMELL: size specified on multiple places
        QPixmap notLoadedPixmap{100, 100};
        notLoadedPixmap.fill(Qt::GlobalColor::lightGray);
        notLoaded = QIcon{notLoadedPixmap};
    }

    int row = index.row();
    if (!myAlbumRepository.isLoaded(row)) {
        if (role == Qt::DisplayRole && !myAlbumRepository.isFiltered()) {
            myAlbumRequests->add(row);
        }
        return notLoaded;
    }

    auto& album = myAlbumRepository.get(row);
    if (index.column() == 0) {
        if (role == Qt::DisplayRole) {
            return QString::fromStdString(album.getName());
        } else {
            if (!album.hasArt()) {
                myArtRequests->add(row);
                return notLoaded;
            } else {
                return QIcon{album.getArt()};
            }
        }
    } else {
        if (role == Qt::DisplayRole) {
            return QString::fromStdString(album.getId());
        } else {
            return QVariant{};
        }
    }
}



int AlbumModel::rowCount(const QModelIndex&) const {
    return myAlbumRepository.maxCount();
}



int AlbumModel::columnCount(const QModelIndex&) const {
    return 2;
}



void AlbumModel::requestAllData() {
    for (int row = 0; row < rowCount(); row++) {
        myAlbumRequests->add(row);
    }
}



void AlbumModel::abortDataRequests() {
    myDataRequestsAborted = true;
    myAlbumRequests->cancel();
    myArtRequests->cancel();
    if (!myAlbumRequests->isInProgress() && !myArtRequests->isInProgress()) {
        dataRequestsAborted();
    }
}



void AlbumModel::onReadyToExecuteAlbums(RequestGroup requestGroup) {
    myAlbumRepository.load(requestGroup.getLower(), requestGroup.getSize());
}



void AlbumModel::onLoaded(pair<int, int>) {
    auto finishedRequestGroup = myAlbumRequests->setFinished();
    if (myAlbumRepository.isFiltered()) {
        beginResetModel();
        endResetModel();
    } else {
        dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
    }

    if (myDataRequestsAborted && !myArtRequests->isInProgress()) {
        dataRequestsAborted();
    }
}



void AlbumModel::onReadyToExecuteArts(RequestGroup requestGroup) {
    myAlbumRepository.loadArts(requestGroup.getLower(), requestGroup.getSize());
}



void AlbumModel::onArtsLoaded(pair<int, int>) {
    auto finishedRequestGroup = myArtRequests->setFinished();
    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));

    if (myDataRequestsAborted && !myAlbumRequests->isInProgress()) {
        dataRequestsAborted();
    }
}



void AlbumModel::onFilterChanged() {
    myArtRequests->cancel();
    beginResetModel();
    endResetModel();
}

}