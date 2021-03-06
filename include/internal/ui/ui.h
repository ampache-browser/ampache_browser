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
#include <QModelIndex>
#include "src/ui/ampache_browser_main_window.h"
#include "infrastructure/event/event.h"
#include "ui/selected_items.h"

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
     * @brief Event fired after user triggered Play function.
     */
    infrastructure::Event<SelectedItems> playTriggered{};

    /**
     * @brief Event fired after user triggered Create Playlist function.
     */
    infrastructure::Event<SelectedItems> createPlaylistTriggered{};

    /**
     * @brief Event fired after user triggered Add To Playlist function.
     */
    infrastructure::Event<SelectedItems> addToPlaylistTriggered{};

    /**
     * @brief Event fired after user selected or unselected artists.
     */
    infrastructure::Event<std::vector<std::string>> artistsSelected{};

    /**
     * @brief Event fired after user selected or unselected albums.
     *
     * @param albumAndArtistIds Pair of selected album IDs and artist IDs.
     */
    infrastructure::Event<std::pair<std::vector<std::string>, std::vector<std::string>>> albumsSelected{};

    /**
     * @brief Event fired after user triggered search function.
     */
    infrastructure::Event<std::string> searchTriggered{};

    /**
     * @brief Event fired after user updated settings.
     *
     * @param Tuple of settings values: Use Demo Server, Server URL, User Name, Password.
     */
    infrastructure::Event<std::tuple<bool, std::string, std::string, std::string>> settingsUpdated{};

    /**
     * @brief Gets main window widged of the application (plugin).
     *
     * @return QWidget*
     */
    QWidget* getMainWidget() const;

    /**
     * @brief Display a notification message to the user in non-intrusive way.
     *
     * @param message The message that shall be displayed.
     */
    void showNotification(const std::string& message);

    /**
     * @brief Populate settings dialog.
     *
     * @note Dialog does not have to be opened.
     */
    void populateSettings(bool useDemoServer, std::string serverUrl, std::string userName);

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
    void onPlayActionTriggered();
    void onCreatePlaylistActionTriggered();
    void onAddToPlaylistActionTriggered();
    void onActivated(const QModelIndex&);
    void onArtistsSelectionModelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onAlbumsSelectionModelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onTracksSelectionModelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onSearchTextChanged(const QString& text);
    void onSearchReturnPressed();
    void onSettingsAccepted();

private:
    // the main window widget
    AmpacheBrowserMainWindow* myMainWindow;

    void enableOrDisablePlayActions();
    SelectedItems getSelectedItems() const;
    QModelIndexList getAristSelectedRows() const;
    QModelIndexList getAlbumsSelectedRows() const;
    QModelIndexList getTracksSelectedRows() const;
};

}



#endif // UI_H
