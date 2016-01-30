// artist_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTISTMODEL_H
#define ARTISTMODEL_H



#include <vector>
#include <unordered_map>
#include <memory>

#include <QAbstractTableModel>

#include "domain/artist.h"
#include "data/artist_repository.h"
#include "src/application/models/requests.h"



namespace application {

class ArtistModel: public QAbstractTableModel {
    Q_OBJECT

public:
    explicit ArtistModel(data::ArtistRepository& artistRepository, QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    data::ArtistRepository& myArtistRepository;
    const std::unique_ptr<Requests> myRequests{new Requests{60}};

    void onReadyToExecute(RequestGroup requestGroup);
    void onReadyArtists(std::pair<int, int> offsetAndLimit);
    void onFilterChanged();
};

}



#endif // ARTISTMODEL_H
