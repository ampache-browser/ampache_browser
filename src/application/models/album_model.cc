// album_model.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <memory>

#include <libaudcore/runtime.h>
#include <libaudcore/i18n.h>
#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtGui/QIcon>

#include "infrastructure/event/delegate.h"
#include "domain/album.h"
#include "data/repositories/album_repository.h"
#include "requests.h"
#include "application/models/album_model.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace data;
using namespace domain;



namespace application {

AlbumModel::AlbumModel(AlbumRepository* const albumRepository, QObject* parent): QAbstractTableModel(parent),
myAlbumRepository(albumRepository) {
    myAlbumRequests->readyToExecute += DELEGATE1(&AlbumModel::onReadyToExecuteAlbums, RequestGroup);
    myAlbumRepository->loaded += DELEGATE1(&AlbumModel::onLoaded, pair<int, int>);
    myArtRequests->readyToExecute += DELEGATE1(&AlbumModel::onReadyToExecuteArts, RequestGroup);
    myAlbumRepository->artsLoaded += DELEGATE1(&AlbumModel::onArtsLoaded, pair<int, int>);
    myAlbumRepository->filterChanged += DELEGATE0(&AlbumModel::onFilterChanged);
    myAlbumRepository->providerChanged += DELEGATE0(&AlbumModel::onProviderChanged);
}



AlbumModel::~AlbumModel() {
    myAlbumRepository->providerChanged -= DELEGATE0(&AlbumModel::onProviderChanged);
    myAlbumRepository->filterChanged -= DELEGATE0(&AlbumModel::onFilterChanged);
    myAlbumRepository->artsLoaded -= DELEGATE1(&AlbumModel::onArtsLoaded, pair<int, int>);
    myArtRequests->readyToExecute -= DELEGATE1(&AlbumModel::onReadyToExecuteArts, RequestGroup);
    myAlbumRepository->loaded -= DELEGATE1(&AlbumModel::onLoaded, pair<int, int>);
    myAlbumRequests->readyToExecute -= DELEGATE1(&AlbumModel::onReadyToExecuteAlbums, RequestGroup);
}



QVariant AlbumModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::DecorationRole)) {
        return QVariant{};
    }

    QVariant notLoaded{};
    if (role == Qt::DisplayRole) {
        notLoaded = QVariant{QString{_("loading...")}};
    } else {
        // SMELL: size specified on multiple places
        QPixmap notLoadedPixmap{100, 100};
        notLoadedPixmap.fill(Qt::GlobalColor::lightGray);
        notLoaded = QIcon{notLoadedPixmap};
    }

    int row = index.row();
    if (!myAlbumRepository->isLoaded(row)) {
        if (role == Qt::DisplayRole && !myAlbumRepository->isFiltered()) {
            myAlbumRequests->add(row);
        }
        return notLoaded;
    }

    auto& album = myAlbumRepository->get(row);
    if (index.column() == 0) {
        if (role == Qt::DisplayRole) {
            return QString::fromStdString(album.getName());
        } else {
            if (!album.hasArt()) {
                if (myIsInUnfilteredArtsLoadMode) {
                    AUDDBG("Removing all art requests and setting unfiltered mode to false.\n");
                    myArtRequests->removeAll();
                    myIsInUnfilteredArtsLoadMode = false;
                }
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
    return myAlbumRepository->maxCount();
}



int AlbumModel::columnCount(const QModelIndex&) const {
    return 2;
}



void AlbumModel::onReadyToExecuteAlbums(RequestGroup requestGroup) {
    myAlbumRepository->load(requestGroup.getLower(), requestGroup.getSize());
}



void AlbumModel::onLoaded(pair<int, int>) {
    auto finishedRequestGroup = myAlbumRequests->setFinished();
    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}



void AlbumModel::onReadyToExecuteArts(RequestGroup requestGroup) {
    if (myIsInUnfilteredArtsLoadMode) {
        myAlbumRepository->loadArtsUnfiltered(requestGroup.getLower(), requestGroup.getSize());
    } else {
        myAlbumRepository->loadArts(requestGroup.getLower(), requestGroup.getSize());
    }
}



void AlbumModel::onArtsLoaded(pair<int, int> offsetAndCount) {
    auto finishedRequestGroup = myArtRequests->setFinished();
    if (myIsInUnfilteredArtsLoadMode) {
        return;
    }

    if (offsetAndCount.second != 0) {
        dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
    }

    if (!myArtRequests->isInProgress()) {

        // there is no more requests from views (via data()) so request here all remaining arts (which were
        // not shown by the view) in an unfiltered manner
        myIsInUnfilteredArtsLoadMode = true;
        requestUnloadedArts();
    }
}



void AlbumModel::onFilterChanged() {
    beginResetModel();

    AUDDBG("Removing all art requests.\n");
    myArtRequests->removeAll();

    // filterChanged event is fired even if it does not results to an actual change of filtered data; it that case
    // if all requests from views (via data()) were already executed, no futher request will be added; therefore the
    // loading of all remaining arts is re-initiated here because all requests were removed above
    if (myIsInUnfilteredArtsLoadMode) {
        requestUnloadedArts();
    }

    endResetModel();
}



void AlbumModel::onProviderChanged() {
    beginResetModel();
    myAlbumRequests->removeAll();
    myArtRequests->removeAll();
    // SMELL: Change of provider keeps the filter set so calling rowCount() here does not guarantees unfiltered count.
    myUnfilteredCount = rowCount();
    requestAllData();
    endResetModel();
}



void AlbumModel::requestAllData() {
    AUDDBG("Requesting all data (%d).\n", myUnfilteredCount);
    for (int row = 0; row < myUnfilteredCount; row++) {
        myAlbumRequests->add(row);
    }
}



void AlbumModel::requestUnloadedArts() {
    AUDDBG("Requesting all unloaded arts.\n");
    for (int row = 0; row < myUnfilteredCount; row++) {
        if (myAlbumRepository->isLoadedUnfiltered(row)) {
            auto& album = myAlbumRepository->getUnfiltered(row);
            if (!album.hasArt()) {
                myArtRequests->add(row);
            }
        }
    }
}

}
