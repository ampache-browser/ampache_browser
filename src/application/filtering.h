// filtering.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef FILTERING_H
#define FILTERING_H



#include <string>
#include <vector>

namespace ui {
class Ui;
}

namespace data {
class ArtistRepository;
class AlbumRepository;
class TrackRepository;
class Indices;
}



namespace application {

/**
 * @brief Filtering logic.
 *
 * Interconnects UI and data model in order to enable filtering.
 */
class Filtering {

public:
    explicit Filtering(ui::Ui& ui, data::ArtistRepository& artistRepository, data::AlbumRepository& albumRepository,
        data::TrackRepository& trackRepository, data::Indices& indices);

    ~Filtering();

private:
    ui::Ui& myUi;
    data::ArtistRepository& myArtistRepository;
    data::AlbumRepository& myAlbumRepository;
    data::TrackRepository& myTrackRepository;
    data::Indices& myIndices;

    void onArtistsSelected(const std::vector<std::string>& ids);
    void onAlbumsSelected(const std::pair<std::vector<std::string>, std::vector<std::string>>& albumAndArtistIds);
    void onSearchTriggered(const std::string& searchText);

    void setArtistFilters(const std::vector<std::string>& ids);
};

}



#endif // FILTERING_H
