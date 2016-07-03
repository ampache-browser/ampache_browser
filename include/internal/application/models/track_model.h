// track_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef TRACKMODEL_H
#define TRACKMODEL_H



#include <memory>
#include <QtCore/QAbstractTableModel>
#include "src/application/models/requests.h"

namespace data {
class TrackRepository;
}



namespace application {

/**
 * @brief Provides tracks data to a QT view.
 */
class TrackModel: public QAbstractTableModel {
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     *
     * @param trackRepository Provides tracks and means to trigger their loading from an external source.
     * @param parent
     */
    explicit TrackModel(data::TrackRepository* const trackRepository, QObject* parent = 0);

    virtual ~TrackModel();

    /**
     * @sa QAbstractTableModel::data()
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @sa QAbstractTableModel::data()
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * @sa QAbstractTableModel::rowCount()
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @sa QAbstractTableModel::columnCount()
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    // stores track repository provided in the constuctor
    data::TrackRepository* const myTrackRepository = nullptr;

    // requests to load tracks from an external source
    const std::unique_ptr<Requests> myRequests{new Requests{60}};

    void onReadyToExecute(RequestGroup requestGroup);
    void onLoaded(std::pair<int, int> offsetAndLimit);
    void onDataSizeOrFilterChanged();
    void onProviderChanged();

    void requestAllData();
};

}



#endif // TRACKMODEL_H
