// track_model.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractTableModel>

#include "infrastructure/event/delegate.h"
#include "domain/track.h"
#include "data/track_repository.h"
#include "requests.h"
#include "application/models/track_model.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace data;
using namespace domain;



namespace application {

TrackModel::TrackModel(TrackRepository& trackRepository, QObject* parent): QAbstractTableModel(parent),
myTrackRepository(trackRepository) {
    myRequests->readyToExecute += DELEGATE1(&TrackModel::onReadyToExecute, RequestGroup);
    myTrackRepository.loaded += DELEGATE1(&TrackModel::onReadyTracks, pair<int, int>);
    myTrackRepository.filterChanged += DELEGATE0(&TrackModel::onFilterChanged);

    // start populating with data
    for (int row = 0; row < rowCount(); row++) {
        myRequests->add(row);
    }
}



QVariant TrackModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant{};
    }

    int row = index.row();
    if (!myTrackRepository.isLoaded(row)) {
        if (role == Qt::DisplayRole && !myTrackRepository.isFiltered()) {
            myRequests->add(row);
        }
        return "...";
    }

    auto& track = myTrackRepository.get(row);
    switch (index.column()) {
        case 0:
            return QString::fromStdString(track.getName());
        case 1:
            return QString::fromStdString(track.getArtist().getName());
        case 2:
            return QString::fromStdString(track.getAlbum().getName());
        default:
            return QVariant{};
    }
}



QVariant TrackModel::headerData(int section, Qt::Orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant{};
    }

    // SMELL: Header names are not translatable.  Should they be pulled from data?
    switch (section) {
        case 0:
            return "Track";
        case 1:
            return "Artist";
        case 2:
            return "Album";
        default:
            return QVariant{};
    }
}



int TrackModel::rowCount(const QModelIndex&) const {
    return myTrackRepository.maxCount();
}



int TrackModel::columnCount(const QModelIndex&) const {
    return 3;
}



void TrackModel::onReadyToExecute(RequestGroup& requestGroup) {
    myTrackRepository.load(requestGroup.getLower(), requestGroup.getSize());
}



void TrackModel::onReadyTracks(pair<int, int>&) {
    auto finishedRequestGroup = myRequests->setFinished();
    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}



void TrackModel::onFilterChanged() {
    beginResetModel();
    endResetModel();
}

}
