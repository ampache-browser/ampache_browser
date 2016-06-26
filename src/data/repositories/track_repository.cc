// track_repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include "domain/track.h"
#include "data/provider_type.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "../data_objects/track_data.h"
#include "data/indices.h"
#include "data/repositories/artist_repository.h"
#include "data/repositories/album_repository.h"
#include "data/repositories/track_repository.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

TrackRepository::TrackRepository(Ampache& ampache, Cache& cache, Indices& indices,
    const ArtistRepository* const artistRepository, const AlbumRepository* const albumRepository):
Repository<TrackData, Track>(ampache, cache, indices),
myArtistRepository(artistRepository),
myAlbumRepository(albumRepository) {
}



int TrackRepository::maxCount() const {
    if (myProviderType == ProviderType::Ampache) {
        return myAmpache.numberOfTracks();
    };
    if (myProviderType == ProviderType::Cache) {
        return myCache.numberOfTracks();
    };
    return 0;
}



void TrackRepository::requestDataLoad(int offset, int limit) {
    myAmpache.requestTracks(offset, limit);
}



Track& TrackRepository::getDomainObject(const data::TrackData& dataItem) const {
    return dataItem.getTrack();
}



Event<pair<vector<unique_ptr<TrackData>>, bool>>& TrackRepository::getDataLoadRequestFinishedEvent() {
    return myAmpache.readyTracks;
}



void TrackRepository::loadDataFromCache() {
    myData = myCache.loadTracksData();
}



void TrackRepository::saveDataToCache() {
    myCache.saveTracksData(myData);
}



void TrackRepository::handleLoadedItem(const TrackData& dataItem) const {
    data::Repository<data::TrackData, domain::Track>::handleLoadedItem(dataItem);

    auto& track = dataItem.getTrack();
    auto& artist = myArtistRepository->getById(dataItem.getArtistId());
    track.setArtist(artist);
    auto& album = myAlbumRepository->getById(dataItem.getAlbumId());
    track.setAlbum(album);
}



void TrackRepository::updateIndices(const vector<unique_ptr<TrackData>>& data) {
    ArtistAlbumsIndex artistAlbums;
    ArtistTracksIndex artistTracks;
    AlbumTracksIndex albumTracks;
    for (auto& dataItem: data) {
        auto& artist = myArtistRepository->getById(dataItem->getArtistId());
        auto& albumData = myAlbumRepository->getAlbumDataById(dataItem->getAlbumId());
        artistAlbums[artist].insert(albumData);
        artistTracks[artist].insert(*dataItem);
        auto& album = albumData.getAlbum();
        albumTracks[album].insert(*dataItem);
    }

    myIndices.updateArtistAlbums(artistAlbums);
    myIndices.updateArtistTracks(artistTracks);
    myIndices.updateAlbumTracks(albumTracks);
}



void TrackRepository::clearIndices() {
    myIndices.clearAlbumsTracks();
    // SMELL: Assuming ArtistsAlbums were already cleared in AlbumRepository (besides, clearing it again here is causin
    // filtered albums dissapear for a moment and then reappear as index is populated by TrackRepository)
    //myIndices.clearArtistsAlbums();
    myIndices.clearArtistsTracks();
}

}
