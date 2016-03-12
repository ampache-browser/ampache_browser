// track_repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <libaudcore/runtime.h>

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



void TrackRepository::requestDataLoad(int offset, int limit) {
    myAmpache.requestTracks(offset, limit);
}



Track& TrackRepository::getDomainObject(const data::TrackData& dataItem) const {
    return dataItem.getTrack();
}



Event<vector<unique_ptr<TrackData>>>& TrackRepository::getDataLoadRequestFinishedEvent() {
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



void TrackRepository::updateIndices(TrackData& dataItem) {
    auto& artist = myArtistRepository->getById(dataItem.getArtistId());
    auto& albumData = myAlbumRepository->getAlbumDataById(dataItem.getAlbumId());
    myIndices.updateArtistAlbums(artist, albumData);
    myIndices.updateArtistTracks(artist, dataItem);
    auto& album = albumData.getAlbum();
    myIndices.updateAlbumTracks(album, dataItem);
}



void TrackRepository::clearIndices() {
    myIndices.clearAlbumsTracks();
    myIndices.clearArtistsAlbums();
    myIndices.clearArtistsTracks();
}



int TrackRepository::getMaxDataSize() const {
    if (myProviderType == ProviderType::Ampache) {
        return myAmpache.numberOfTracks();
    };
    if (myProviderType == ProviderType::Cache) {
        return myCache.numberOfTracks();
    };
    return 0;
}

}
