// ui.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



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

class Ui: QObject {
    Q_OBJECT

public:
    explicit Ui();

    Ui(const Ui& other) = delete;

    Ui& operator=(const Ui& other) = delete;

//     Event<SelectedItems> playTriggered;

    infrastructure::Event<bool> albumWindowRedraw{};

    infrastructure::Event<std::vector<std::string>> artistsSelected{};

    infrastructure::Event<std::vector<std::string>> albumsSelected{};

    infrastructure::Event<std::string> searchTriggered{};

    void setArtistModel(QAbstractItemModel& model);

    void setAlbumModel(QAbstractItemModel& model);

    void setTrackModel(QAbstractItemModel& model);

    void setSearchCompletionModel(QAbstractItemModel& model);

private slots:
    void onPlayActionTriggered();
    void onArtistsSelectionModelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onAlbumsSelectionModelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onSearchTextChanged(const QString& text);
    void onSearchReturnPressed();

private:
    const std::unique_ptr<AmpacheBrowserMainWindow> myMainWindow;
};

}



#endif // UI_H
