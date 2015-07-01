// track_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef TRACKMODEL_H
#define TRACKMODEL_H



#include <vector>
#include <unordered_map>
#include <memory>

#include <QtCore/QAbstractTableModel>

#include "domain/track.h"
#include "data/track_repository.h"
#include "src/application/models/requests.h"



namespace application {

class TrackModel: public QAbstractTableModel {
    Q_OBJECT

public:
    explicit TrackModel(data::TrackRepository& trackRepository, QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    data::TrackRepository& myTrackRepository;
    const std::unique_ptr<Requests> myRequests{new Requests{60}};

    void onReadyToExecute(RequestGroup& requestGroup);
    void onReadyTracks(std::pair<int, int>& offsetAndLimit);
};

}



#endif // TRACKMODEL_H
