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

using namespace std;
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
    myUi.artistsSelected += DELEGATE1(&Filtering::onArtistsSelected, vector<string>);
    myUi.albumsSelected += DELEGATE1(&Filtering::onAlbumsSelected, pair<vector<string>, vector<string>>);
    myUi.searchTriggered += DELEGATE1(&Filtering::onSearchTriggered, string);
}



Filtering::~Filtering() {
    myUi.searchTriggered -= DELEGATE1(&Filtering::onSearchTriggered, string);
    myUi.albumsSelected -= DELEGATE1(&Filtering::onAlbumsSelected, pair<vector<string>, vector<string>>);
    myUi.artistsSelected -= DELEGATE1(&Filtering::onArtistsSelected, vector<string>);
}



void Filtering::onArtistsSelected(const vector<string>& ids) {
    if (ids.empty()) {
        myAlbumRepository.unsetFilter();
        myTrackRepository.unsetFilter();
    } else {
        setArtistFilters(ids);
    }
}



void Filtering::onAlbumsSelected(const pair<vector<string>, vector<string>>& albumAndArtistIds) {
    if (albumAndArtistIds.first.empty()) {
        if (albumAndArtistIds.second.empty()) {
            myTrackRepository.unsetFilter();
        } else {
            setArtistFilters(albumAndArtistIds.second);
        }
    } else {
        vector<reference_wrapper<const Album>> albums;
        for (auto& id: albumAndArtistIds.first) {
            auto album = myAlbumRepository.getById(id);
            albums.push_back(*album);
        }
        myTrackRepository.setFilter(unique_ptr<Filter<TrackData>>{new AlbumFilterForTracks{albums, myIndices}});
    }
}



void Filtering::onSearchTriggered(const string& searchText) {
    if (searchText.empty()) {
        myArtistRepository.unsetFilter();
        myAlbumRepository.unsetFilter();
        myTrackRepository.unsetFilter();
    } else {
        myArtistRepository.setFilter(unique_ptr<Filter<ArtistData>>{new NameFilterForArtists{searchText}});
        myAlbumRepository.setFilter(unique_ptr<Filter<AlbumData>>{new NameFilterForAlbums{searchText}});
        myTrackRepository.setFilter(unique_ptr<Filter<TrackData>>{new NameFilterForTracks{searchText}});
    }
}



void Filtering::setArtistFilters(const vector<string>& ids) {
    vector<reference_wrapper<const Artist>> artists;
    for (auto& id: ids) {
        auto artist = myArtistRepository.getById(id);
        artists.push_back(*artist);
    }
    myAlbumRepository.setFilter(unique_ptr<Filter<AlbumData>>{new ArtistFilterForAlbums{artists, myIndices}});
    myTrackRepository.setFilter(unique_ptr<Filter<TrackData>>{new ArtistFilterForTracks{artists, myIndices}});
}

}
