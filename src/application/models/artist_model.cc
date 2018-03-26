// artist_model.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2018 Róbert Čerňanský



#include <QtCore/QVariant>
#include <QtCore/QModelIndex>

#include "infrastructure/event/delegate.h"
#include "infrastructure/logging/logging.h"
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
    myArtistRepository->dataSizeChanged += DELEGATE0(&ArtistModel::onDataSizeOrFilterChanged);
    myArtistRepository->filterChanged += DELEGATE0(&ArtistModel::onDataSizeOrFilterChanged);
    myArtistRepository->providerChanged += DELEGATE0(&ArtistModel::onProviderChanged);
}



ArtistModel::~ArtistModel() {
    myArtistRepository->providerChanged -= DELEGATE0(&ArtistModel::onProviderChanged);
    myArtistRepository->filterChanged -= DELEGATE0(&ArtistModel::onDataSizeOrFilterChanged);
    myArtistRepository->dataSizeChanged -= DELEGATE0(&ArtistModel::onDataSizeOrFilterChanged);
    myArtistRepository->loaded -= DELEGATE1(&ArtistModel::onLoaded, pair<int, int>);
    myRequests->readyToExecute -= DELEGATE1(&ArtistModel::onReadyToExecute, RequestGroup);
}



QVariant ArtistModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
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
    return myArtistRepository->count();
}



int ArtistModel::columnCount(const QModelIndex&) const {
    return 2;
}



void ArtistModel::onReadyToExecute(RequestGroup requestGroup) {
    myArtistRepository->load(requestGroup.getLower(), requestGroup.getSize());
}



void ArtistModel::onLoaded(pair<int, int> offsetAndLimit) {
    myRequests->setFinished(offsetAndLimit.first, offsetAndLimit.second);
    dataChanged(createIndex(offsetAndLimit.first, 0), createIndex(offsetAndLimit.first + offsetAndLimit.second - 1, 0));
}



void ArtistModel::onDataSizeOrFilterChanged() {
    beginResetModel();
    endResetModel();
}



void ArtistModel::onProviderChanged() {
    beginResetModel();
    myRequests->removeAll();
    requestAllData();
    endResetModel();
}



void ArtistModel::requestAllData() {
    LOG_DBG("Requesting all data.");
    for (int row = 0; row < myArtistRepository->maxCount(); row++) {
        myRequests->add(row);
    }
}

}
