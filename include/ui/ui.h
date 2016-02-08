// ui.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef UI_H
#define UI_H



#include <memory>
#include <vector>
#include <QObject>
#include "src/ui/ampache_browser_main_window.h"
#include "infrastructure/event/event.h"

class QAbstractItemModel;
class QItemSelection;



namespace ui {

/**
 * @brief Provides access to the user interface.
 */
class Ui: QObject {
    Q_OBJECT

public:
    explicit Ui();

    Ui(const Ui& other) = delete;

    Ui& operator=(const Ui& other) = delete;

    /**
     * @brief Event fired after user triggered play function.
     */
    infrastructure::Event<std::vector<std::string>> playTriggered{};

    /**
     * @brief Event fired after user selected or unselected artists.
     */
    infrastructure::Event<std::vector<std::string>> artistsSelected{};

    /**
     * @brief Event fired after user selected or unselected albums.
     */
    infrastructure::Event<std::vector<std::string>> albumsSelected{};

    /**
     * @brief Event fired after user triggered search function.
     */
    infrastructure::Event<std::string> searchTriggered{};

    /**
     * @brief Gets main window widged of the application (plugin).
     *
     * @return QWidget*
     */
    QWidget* getMainWidget() const;

    /**
     * @brief Sets artist model to the corresponding UI view.
     *
     * @param model
     */
    void setArtistModel(QAbstractItemModel& model);

    /**
     * @brief Sets album model to the corresponding UI view.
     *
     * @param model
     */
    void setAlbumModel(QAbstractItemModel& model);

    /**
     * @brief Sets track model to the corresponding UI view.
     *
     * @param model
     */
    void setTrackModel(QAbstractItemModel& model);

private slots:
    void onPlayActionTriggered() const;
    void onActivated(const QModelIndex& index) const;
    void onArtistsSelectionModelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onAlbumsSelectionModelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onSearchTextChanged(const QString& text);
    void onSearchReturnPressed();

private:
    // the main window widget
    AmpacheBrowserMainWindow* myMainWindow;

    void raisePlayTriggeredForSelectedTracks() const;
};

}



#endif // UI_H
