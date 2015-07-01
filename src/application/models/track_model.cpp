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
#include "data/track_repository.h"
#include "requests.h"
#include "application/models/track_model.h"

using namespace std;
using namespace placeholders;
using namespace data;
using namespace domain;



namespace application {

TrackModel::TrackModel(TrackRepository& trackRepository, QObject* parent): QAbstractTableModel(parent),
myTrackRepository(trackRepository) {
    myRequests->readyToExecute += bind(&TrackModel::onReadyToExecute, this, _1);
    myTrackRepository.loaded += bind(&TrackModel::onReadyTracks, this, _1);

    // start populating with data
    for (int row; row < rowCount(); row++) {
        myRequests->add(row);
    }
}



QVariant TrackModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant{};
    }
    if (role != Qt::DisplayRole) {
        return QVariant{};
    }

    // TODO: Implement artist (and album) columns.
    if (index.column() == 1)
    {
        return "col1";
    }

    int row = index.row();
    if (!myTrackRepository.isLoaded(row)) {
        if (role == Qt::DisplayRole) {
            myRequests->add(row);
        }
        return "...";
    }

    auto& track = myTrackRepository.get(row);
    return QString::fromStdString(track.getName());
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
        default:
            return QVariant{};
    }
}



int TrackModel::rowCount(const QModelIndex&) const {
    return myTrackRepository.maxCount();
}



int TrackModel::columnCount(const QModelIndex&) const {
    return 2;
}



void TrackModel::onReadyToExecute(RequestGroup& requestGroup) {
    myTrackRepository.load(requestGroup.getLower(), requestGroup.getSize());
}



void TrackModel::onReadyTracks(pair<int, int>&) {
    auto finishedRequestGroup = myRequests->setFinished();
    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}

}
