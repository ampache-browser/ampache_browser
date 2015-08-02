// names_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef NAMESMODEL_H
#define NAMESMODEL_H



#include <vector>
#include <QAbstractListModel>
#include <QModelIndex>
#include "data/artist_repository.h"
#include "data/album_repository.h"
#include "data/track_repository.h"



namespace application {

class NamesModel: public QAbstractListModel {
    Q_OBJECT

public:
    explicit NamesModel(data::ArtistRepository& artistRepository, data::AlbumRepository& albumRepository,
        data::TrackRepository& trackRepository, QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    data::ArtistRepository& myArtistRepository;
    data::AlbumRepository& myAlbumRepository;
    data::TrackRepository& myTrackRepository;

    std::vector<std::string> myNames;

    void populate();
};

}



#endif // NAMESMODEL_H
