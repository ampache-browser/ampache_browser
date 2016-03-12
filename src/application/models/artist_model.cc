// artist_model.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <libaudcore/runtime.h>
#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QAbstractTableModel>

#include "infrastructure/event/delegate.h"
#include "domain/artist.h"
#include "data//repositories/artist_repository.h"
#include "requests.h"
#include "application/models/artist_model.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace data;
using namespace domain;



namespace application {

ArtistModel::ArtistModel(ArtistRepository* const artistRepository, QObject* parent): QAbstractTableModel(parent),
myArtistRepository(artistRepository) {
    myRequests->readyToExecute += DELEGATE1(&ArtistModel::onReadyToExecute, RequestGroup);
    myArtistRepository->loaded += DELEGATE1(&ArtistModel::onLoaded, pair<int, int>);
    myArtistRepository->filterChanged += DELEGATE0(&ArtistModel::onFilterChanged);
    myArtistRepository->providerChanged += DELEGATE0(&ArtistModel::onProviderChanged);
}



ArtistModel::~ArtistModel() {
    myArtistRepository->providerChanged -= DELEGATE0(&ArtistModel::onProviderChanged);
    myArtistRepository->filterChanged -= DELEGATE0(&ArtistModel::onFilterChanged);
    myArtistRepository->loaded -= DELEGATE1(&ArtistModel::onLoaded, pair<int, int>);
    myRequests->readyToExecute -= DELEGATE1(&ArtistModel::onReadyToExecute, RequestGroup);
}



QVariant ArtistModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole || myDataRequestsAborted) {
        return QVariant{};
    }

    int row = index.row();
    if (!myArtistRepository->isLoaded(row)) {
        if (role == Qt::DisplayRole && !myArtistRepository->isFiltered()) {
            myRequests->add(row);
        }
        return "...";
    }

    auto& artist = myArtistRepository->get(row);
    if (index.column() == 0) {
        return QString::fromStdString(artist.getName());
    }
    else {
        return QString::fromStdString(artist.getId());
    }
}



int ArtistModel::rowCount(const QModelIndex&) const {
    return myArtistRepository->maxCount();
}



int ArtistModel::columnCount(const QModelIndex&) const {
    return 2;
}



void ArtistModel::requestAllData() {
    AUDDBG("Requesting all data (%d).\n", rowCount());
    for (int row = 0; row < rowCount(); row++) {
        myRequests->add(row);
    }
}



void ArtistModel::abortDataRequests() {
    myDataRequestsAborted = true;
    myRequests->removeAll();
    if (!myRequests->isInProgress()) {
        dataRequestsAborted();
    }
}



void ArtistModel::onReadyToExecute(RequestGroup requestGroup) {
    myArtistRepository->load(requestGroup.getLower(), requestGroup.getSize());
}



void ArtistModel::onLoaded(pair<int, int>) {
    auto finishedRequestGroup = myRequests->setFinished();
    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));

    if (myDataRequestsAborted) {
        dataRequestsAborted();
    }
}



void ArtistModel::onFilterChanged() {
    beginResetModel();
    endResetModel();
}



void ArtistModel::onProviderChanged() {
    myRequests->removeAll();
    myRequests->cancelCurrent();
    requestAllData();
    beginResetModel();
    endResetModel();
}

}
