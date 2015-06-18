// track_model.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractTableModel>

#include "domain/track.h"
#include "../ampache_service.h"
#include "requests.h"
#include "application/models/track_model.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace application {

TrackModel::TrackModel(AmpacheService& ampacheService, QObject* parent): QAbstractTableModel(parent),
myAmpacheService(ampacheService) {
    myRequests->readyToExecute += bind(&TrackModel::onReadyToExecute, this, _1);
    myAmpacheService.readyTracks += bind(&TrackModel::onReadyTracks, this, _1);

    // start populating with data
    for (int row; row < rowCount(); row++) {
        myRequests->add(row);
    }
}



TrackModel::~TrackModel() {
    delete(myRequests);
}



QVariant TrackModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant{};
    }
    if (role != Qt::DisplayRole) {
        return QVariant{};
    }

    int row = index.row();
    auto indexAndTrack = myTracks.find(row);
    if (indexAndTrack == myTracks.end()) {
        myRequests->add(row);
        return QVariant{QString{"..."}};
    }
    return QVariant{QString::fromStdString(indexAndTrack->second->getName())};
}



int TrackModel::rowCount(const QModelIndex&) const {
    return myAmpacheService.numberOfTracks();
}



int TrackModel::columnCount(const QModelIndex&) const {
    return 1;
}



void TrackModel::onReadyToExecute(RequestGroup& requestGroup) {
    myAmpacheService.requestTracks(requestGroup.getLower(), requestGroup.getSize());
}



void TrackModel::onReadyTracks(vector<unique_ptr<Track>>& tracks) {
    auto finishedRequestGroup = myRequests->setFinished();

    int row = finishedRequestGroup.getLower();
    for (auto& track: tracks) {
        myTracks[row++] = move(track);
    }

    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}

}
