// artist_model.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QAbstractTableModel>

#include "infrastructure/event/delegate.h"
#include "domain/artist.h"
#include "data/artist_repository.h"
#include "requests.h"
#include "application/models/artist_model.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace data;
using namespace domain;



namespace application {

ArtistModel::ArtistModel(ArtistRepository& artistRepository, QObject* parent): QAbstractTableModel(parent),
myArtistRepository(artistRepository) {
    myRequests->readyToExecute += DELEGATE1(&ArtistModel::onReadyToExecute, RequestGroup);
    myArtistRepository.loaded += DELEGATE1(&ArtistModel::onReadyArtists, pair<int, int>);
    myArtistRepository.filterChanged += DELEGATE0(&ArtistModel::onFilterChanged);

    // start populating with data
    for (int row = 0; row < rowCount(); row++) {
        myRequests->add(row);
    }
}



QVariant ArtistModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant{};
    }
    if (role != Qt::DisplayRole) {
        return QVariant{};
    }

    int row = index.row();
    if (!myArtistRepository.isLoaded(row)) {
        if (role == Qt::DisplayRole && !myArtistRepository.isFiltered()) {
            myRequests->add(row);
        }
        return "...";
    }

    auto& artist = myArtistRepository.get(row);
    if (index.column() == 0) {
        return QString::fromStdString(artist.getName());
    }
    else {
        return QString::fromStdString(artist.getId());
    }
}



int ArtistModel::rowCount(const QModelIndex&) const {
    return myArtistRepository.maxCount();
}



int ArtistModel::columnCount(const QModelIndex&) const {
    return 2;
}



void ArtistModel::onReadyToExecute(RequestGroup& requestGroup) {
    myArtistRepository.load(requestGroup.getLower(), requestGroup.getSize());
}



void ArtistModel::onReadyArtists(pair<int, int>&) {
    auto finishedRequestGroup = myRequests->setFinished();
    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}



void ArtistModel::onFilterChanged() {
    beginResetModel();
    endResetModel();
}

}
