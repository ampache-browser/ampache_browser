// filtering.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "infrastructure/event/delegate.h"
#include "domain/artist.h"
#include "data/filters/filter.h"
#include "data/filters/name_filter_for_artists.h"
#include "data/filters/artist_filter_for_albums.h"
#include "data/filters/name_filter_for_albums.h"
#include "data/filters/artist_filter_for_tracks.h"
#include "data/filters/album_filter_for_tracks.h"
#include "data/filters/name_filter_for_tracks.h"
#include "data/repositories/artist_repository.h"
#include "data/repositories/album_repository.h"
#include "data/repositories/track_repository.h"
#include "ui/ui.h"
#include "filtering.h"

namespace domain {
class Album;
}

namespace data {
class AlbumData;
class ArtistData;
class TrackData;
}

using namespace infrastructure;
using namespace domain;
using namespace data;
using namespace ui;



namespace application {

Filtering::Filtering(Ui& ui, ArtistRepository& artistRepository, AlbumRepository& albumRepository,
    TrackRepository& trackRepository, Indices& indices):
myUi(ui),
myArtistRepository(artistRepository),
myAlbumRepository(albumRepository),
myTrackRepository(trackRepository),
myIndices(indices) {
    myUi.artistsSelected += DELEGATE1(&Filtering::onArtistsSelected, std::vector<std::string>);
    myUi.albumsSelected += DELEGATE1(
        &Filtering::onAlbumsSelected, std::pair<std::vector<std::string>, std::vector<std::string>>);
    myUi.searchTriggered += DELEGATE1(&Filtering::onSearchTriggered, std::string);
}



Filtering::~Filtering() {
    myUi.searchTriggered -= DELEGATE1(&Filtering::onSearchTriggered, std::string);
    myUi.albumsSelected -= DELEGATE1(
        &Filtering::onAlbumsSelected, std::pair<std::vector<std::string>, std::vector<std::string>>);
    myUi.artistsSelected -= DELEGATE1(&Filtering::onArtistsSelected, std::vector<std::string>);
}



void Filtering::onArtistsSelected(const std::vector<std::string>& ids) {
    if (ids.empty()) {
        myAlbumRepository.unsetFilter();
        myTrackRepository.unsetFilter();
    } else {
        setArtistFilters(ids);
    }
}



void Filtering::onAlbumsSelected(const std::pair<std::vector<std::string>,
                                 std::vector<std::string>>& albumAndArtistIds) {
    if (albumAndArtistIds.first.empty()) {
        if (albumAndArtistIds.second.empty()) {
            myTrackRepository.unsetFilter();
        } else {
            setArtistFilters(albumAndArtistIds.second);
        }
    } else {
        std::vector<std::reference_wrapper<const Album>> albums;
        for (auto& id: albumAndArtistIds.first) {
            auto album = myAlbumRepository.getById(id);
            albums.push_back(*album);
        }
        myTrackRepository.setFilter(std::unique_ptr<Filter<TrackData>>{new AlbumFilterForTracks{albums, myIndices}});
    }
}



void Filtering::onSearchTriggered(const std::string& searchText) {
    if (searchText.empty()) {
        myArtistRepository.unsetFilter();
        myAlbumRepository.unsetFilter();
        myTrackRepository.unsetFilter();
    } else {
        myArtistRepository.setFilter(std::unique_ptr<Filter<ArtistData>>{new NameFilterForArtists{searchText}});
        myAlbumRepository.setFilter(std::unique_ptr<Filter<AlbumData>>{new NameFilterForAlbums{searchText}});
        myTrackRepository.setFilter(std::unique_ptr<Filter<TrackData>>{new NameFilterForTracks{searchText}});
    }
}



void Filtering::setArtistFilters(const std::vector<std::string>& ids) {
    std::vector<std::reference_wrapper<const Artist>> artists;
    for (auto& id: ids) {
        auto artist = myArtistRepository.getById(id);
        artists.push_back(*artist);
    }
    myAlbumRepository.setFilter(std::unique_ptr<Filter<AlbumData>>{new ArtistFilterForAlbums{artists, myIndices}});
    myTrackRepository.setFilter(std::unique_ptr<Filter<TrackData>>{new ArtistFilterForTracks{artists, myIndices}});
}

}
