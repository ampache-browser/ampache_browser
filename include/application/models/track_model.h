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
#include "src/application/models/requests.h"



namespace application {

class AmpacheService;



class TrackModel: public QAbstractTableModel {
    Q_OBJECT

public:
    explicit TrackModel(AmpacheService& ampacheService, QObject* parent = 0);

    ~TrackModel() override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    AmpacheService& myAmpacheService;
    std::unordered_map<int, std::unique_ptr<domain::Track>> myTracks;
    Requests* const myRequests = new Requests{60};

    void onReadyToExecute(RequestGroup& requestGroup);
    void onReadyTracks(std::vector<std::unique_ptr<domain::Track>>& tracks);
};

}



#endif // TRACKMODEL_H
