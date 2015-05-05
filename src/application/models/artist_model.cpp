// artist_model.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <iostream>

#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractListModel>

#include "domain/artist.h"
#include "../ampache_service.h"
#include "requests.h"
#include "application/models/artist_model.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace application {

ArtistModel::ArtistModel(AmpacheService& ampacheService, QObject* parent): QAbstractListModel(parent),
myAmpacheService(ampacheService) {
    myRequests->readyToExecute += bind(&ArtistModel::onReadyToExecute, this, _1);
    myAmpacheService.readyArtists += bind(&ArtistModel::onReadyArtists, this, _1);

    // start populating with data
    for (int row; row < rowCount(); row++) {
        myRequests->add(row);
    }
}



ArtistModel::~ArtistModel() {
    delete(myRequests);
}



QVariant ArtistModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant{};
    }
    if (role != Qt::DisplayRole) {
        return QVariant{};
    }

    int row = index.row();
    auto indexAndArtist = myArtists.find(row);
    if (indexAndArtist == myArtists.end()) {
        myRequests->add(row);
        return QVariant{QString{"..."}};
    }
    return QVariant{QString::fromStdString(indexAndArtist->second->getName())};
}



int ArtistModel::rowCount(const QModelIndex&) const {
    return myAmpacheService.numberOfArtists();
}



void ArtistModel::onReadyToExecute(RequestGroup& requestGroup) {
    myAmpacheService.requestArtists(requestGroup.getLower(), requestGroup.getSize());
}



void ArtistModel::onReadyArtists(vector<unique_ptr<Artist>>& artists) {
    auto finishedRequestGroup = myRequests->setFinished();

    int row = finishedRequestGroup.getLower();
    for (auto& artist: artists) {
        myArtists[row++] = move(artist);
    }

    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}

}
