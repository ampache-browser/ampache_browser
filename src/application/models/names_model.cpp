// names_model.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QAbstractListModel>
#include <QModelIndex>
#include "application/models/names_model.h"

using namespace std;
using namespace domain;
using namespace data;



namespace application {

NamesModel::NamesModel(ArtistRepository& artistRepository, AlbumRepository& albumRepository,
    TrackRepository& trackRepository, QObject* parent): QAbstractListModel(parent),
myArtistRepository(artistRepository),
myAlbumRepository(albumRepository),
myTrackRepository(trackRepository) {

    populate();
}



QVariant NamesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant{};
    }

    return QString::fromStdString(myNames[index.row()]);
}



int NamesModel::rowCount(const QModelIndex&) const {
    return myNames.size();
}



void NamesModel::populate() {
    for (Artist& artist: myArtistRepository.getAll()) {
        myNames.push_back(artist.getName());
    }
    for (Album& album: myAlbumRepository.getAll()) {
        myNames.push_back(album.getName());
    }
    for (Track& track: myTrackRepository.getAll()) {
        myNames.push_back(track.getName());
    }
}

}
