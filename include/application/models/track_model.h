// track_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef TRACKMODEL_H
#define TRACKMODEL_H



#include <vector>
#include <unordered_map>
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
     * @brief Fired when loading tracks from an external source has been aborted.
     *
     * @sa abortDataRequests()
     */
    infrastructure::Event<void> dataRequestsAborted{};

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

    /**
     * @brief Tell the model to request load of all tracks from an external source.
     */
    void requestAllData();

    /**
     * @brief Tell the model to abort all pending requests for loading tracks from an external source.
     *
     * The ::dataRequestsAborted event is fired after the requests has been aborted.  After calling this method the
     * model will not perform any further attempts to load tracks.  The method is intended to support
     * the graceful termination of the application.
     */
    void abortDataRequests();

private:
    // stores track repository provided in the constuctor
    data::TrackRepository* const myTrackRepository = nullptr;

    // requests to load tracks from an external source
    const std::unique_ptr<Requests> myRequests{new Requests{60}};

    // true if abortDataRequests() was called
    bool myDataRequestsAborted = false;

    void onReadyToExecute(RequestGroup requestGroup);
    void onLoaded(std::pair<int, int> offsetAndLimit);
    void onFilterChanged();
    void onProviderChanged();
};

}



#endif // TRACKMODEL_H
