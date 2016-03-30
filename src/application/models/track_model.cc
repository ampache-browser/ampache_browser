// track_model.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <libaudcore/runtime.h>
#include <libaudcore/i18n.h>
#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractTableModel>

#include "infrastructure/event/delegate.h"
#include "domain/track.h"
#include "data/repositories/track_repository.h"
#include "requests.h"
#include "application/models/track_model.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace data;
using namespace domain;



namespace application {

TrackModel::TrackModel(data::TrackRepository* const trackRepository, QObject* parent): QAbstractTableModel(parent),
myTrackRepository(trackRepository) {
    myRequests->readyToExecute += DELEGATE1(&TrackModel::onReadyToExecute, RequestGroup);
    myTrackRepository->loaded += DELEGATE1(&TrackModel::onLoaded, pair<int, int>);
    myTrackRepository->filterChanged += DELEGATE0(&TrackModel::onFilterChanged);
    myTrackRepository->providerChanged += DELEGATE0(&TrackModel::onProviderChanged);
}



TrackModel::~TrackModel() {
    myTrackRepository->providerChanged -= DELEGATE0(&TrackModel::onProviderChanged);
    myTrackRepository->filterChanged -= DELEGATE0(&TrackModel::onFilterChanged);
    myTrackRepository->loaded -= DELEGATE1(&TrackModel::onLoaded, pair<int, int>);
    myRequests->readyToExecute -= DELEGATE1(&TrackModel::onReadyToExecute, RequestGroup);
}



QVariant TrackModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant{};
    }
    int row = index.row();
    int column = index.column();
    if (!myTrackRepository->isLoaded(row)) {
        if (role == Qt::DisplayRole && column == 0 && !myTrackRepository->isFiltered()) {
            myRequests->add(row);
        }
        return "...";
    }

    auto& track = myTrackRepository->get(row);
    switch (column) {
        case 0:
            return QString::fromStdString(track.getName());
        case 1:
            return QString::fromStdString(track.getArtist().getName());
        case 2:
            return QString::fromStdString(track.getAlbum().getName());
        case 3:
            return QString::fromStdString(track.getId());
        default:
            return QVariant{};
    }
}



QVariant TrackModel::headerData(int section, Qt::Orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant{};
    }

    switch (section) {
        case 0:
            return _("Track");
        case 1:
            return _("Artist");
        case 2:
            return _("Album");
        default:
            return QVariant{};
    }
}



int TrackModel::rowCount(const QModelIndex&) const {
    return myTrackRepository->count();
}



int TrackModel::columnCount(const QModelIndex&) const {
    return 4;
}



void TrackModel::onReadyToExecute(RequestGroup requestGroup) {
    myTrackRepository->load(requestGroup.getLower(), requestGroup.getSize());
}



void TrackModel::onLoaded(pair<int, int>) {
    auto finishedRequestGroup = myRequests->setFinished();
    dataChanged(createIndex(finishedRequestGroup.getLower(), 0), createIndex(finishedRequestGroup.getUpper(), 0));
}



void TrackModel::onFilterChanged() {
    beginResetModel();
    endResetModel();
}



void TrackModel::onProviderChanged() {
    beginResetModel();
    myRequests->removeAll();
    requestAllData();
    endResetModel();
}



void TrackModel::requestAllData() {
    AUDDBG("Requesting all data.\n");
    for (int row = 0; row < myTrackRepository->maxCount(); row++) {
        myRequests->add(row);
    }
}

}
