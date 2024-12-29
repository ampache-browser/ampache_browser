// cache.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <stdio.h>
#include <vector>
#include <memory>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <utility>
#include <functional>
#include <map>
#include <string>

#include <QObject>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QFutureWatcher>
#include <QFutureIterator>
#include <QtConcurrent/QtConcurrent>

#include "infrastructure/logging/logging.h"
#include "infrastructure/filesystem.h"
#include "domain/artist.h"
#include "domain/album.h"
#include "domain/track.h"
#include "../data_objects/artist_data.h"
#include "../data_objects/album_data.h"
#include "../data_objects/track_data.h"
#include "data/providers/cache.h"

using namespace infrastructure;
using namespace domain;



namespace data {

#ifdef _WIN32
#define FSPATH(pathName) filesystem::path{StringEncoding::utf8ToWide(pathName)}
#else
#define FSPATH(pathName) filesystem::path{pathName}
#endif



/**
 * @warning Class expects that all save* methods will be called subsequently.
 */
Cache::Cache(const std::string& serverUrl, const std::string& user):
myCurrentServerUrl{serverUrl},
myCurrentUser{user} {
    if (!Filesystem::isDirExisting(ALBUM_ARTS_DIR)) {
        Filesystem::makePath(ALBUM_ARTS_DIR, 0700);
        // TODO: Handle errors.
    }
    std::ifstream metaStream{std::FSPATH(META_PATH)};
    if (!metaStream) {
        invalidate();
    } else {
        if (!loadMeta(metaStream)) {
            invalidate();
        }
    }
}



std::chrono::system_clock::time_point Cache::getLastUpdate() const {
    return myLastUpdate;
}



std::string Cache::getServerUrl() const {
    return myServerUrl;
}



std::string Cache::getUser() const {
    return myUser;
}



int Cache::numberOfArtists() const {
    return myNumberOfArtists;
}



int Cache::numberOfAlbums() const {
    return myNumberOfAlbums;
}



int Cache::numberOfTracks() const {
    return myNumberOfTracks;
}



std::vector<std::unique_ptr<ArtistData>> Cache::loadArtistsData() const {
    std::vector<std::unique_ptr<ArtistData>> artistsData{};
    std::ifstream artistsDataStream{std::FSPATH(ARTISTS_DATA_PATH), std::ios::binary };
    int count = 0;
    artistsDataStream.read(reinterpret_cast<char*>(&count), sizeof count);
    for (int idx = 0; idx < count; idx++) {
        auto id = readString(artistsDataStream);
        int numberOfAlbums = 0;
        artistsDataStream.read(reinterpret_cast<char*>(&numberOfAlbums), sizeof numberOfAlbums);
        int numberOfTracks = 0;
        artistsDataStream.read(reinterpret_cast<char*>(&numberOfTracks), sizeof numberOfTracks);

        auto name = readString(artistsDataStream);

        artistsData.emplace_back(
            new ArtistData{id, numberOfAlbums, numberOfTracks, std::unique_ptr<Artist>{new Artist{id, name}}});
    }

    return artistsData;
}



std::vector<std::unique_ptr<AlbumData>> Cache::loadAlbumsData() const {
    std::vector<std::unique_ptr<AlbumData>> albumsData{};

    std::ifstream albumsDataStream{std::FSPATH(ALBUMS_DATA_PATH), std::ios::binary };
    int count = 0;
    albumsDataStream.read(reinterpret_cast<char*>(&count), sizeof count);
    for (int idx = 0; idx < count; idx++) {
        auto id = readString(albumsDataStream);
        auto artUrl = readString(albumsDataStream);
        auto artistId = readString(albumsDataStream);
        int numberOfTracks = 0;
        albumsDataStream.read(reinterpret_cast<char*>(&numberOfTracks), sizeof numberOfTracks);

        auto name = readString(albumsDataStream);
        int releaseYear = 0;
        albumsDataStream.read(reinterpret_cast<char*>(&releaseYear), sizeof releaseYear);
        int mediaNumber = 0;
        albumsDataStream.read(reinterpret_cast<char*>(&mediaNumber), sizeof mediaNumber);

        albumsData.emplace_back(
            new AlbumData{id, artUrl, artistId, numberOfTracks, std::unique_ptr<Album>{
                new Album{id, name, releaseYear, mediaNumber}}});
    }

    return albumsData;
}



std::vector<std::unique_ptr<TrackData>> Cache::loadTracksData() const {
    std::vector<std::unique_ptr<TrackData>> tracksData{};

    std::ifstream tracksDataStream{std::FSPATH(TRACKS_DATA_PATH), std::ios::binary };
    int count = 0;
    tracksDataStream.read(reinterpret_cast<char*>(&count), sizeof count);
    for (int idx = 0; idx < count; idx++) {
        auto id = readString(tracksDataStream);
        auto artistId = readString(tracksDataStream);
        auto albumId = readString(tracksDataStream);

        auto name = readString(tracksDataStream);
        auto disk = readString(tracksDataStream);
        int number = 0;
        tracksDataStream.read(reinterpret_cast<char*>(&number), sizeof number);
        auto url = readString(tracksDataStream);

        tracksData.emplace_back(
            new TrackData{id, artistId, albumId, std::unique_ptr<Track>{new Track{id, name, disk, number, url}}});
    }

    return tracksData;
}



void Cache::requestAlbumArts(const std::vector<std::string>& ids) {
    LOG_DBG("Getting %d album arts.", ids.size());
    myRequestedAlbumArtIds = ids;
    auto artsLoadFutureWatcher = new QFutureWatcher<std::pair<std::string, QImage>>();
    connect(artsLoadFutureWatcher, SIGNAL(finished()), this, SLOT(onArtsLoadFinished()));
    artsLoadFutureWatcher->setFuture(QtConcurrent::mapped(myRequestedAlbumArtIds,
        bind(&Cache::loadAlbumArt, this, std::placeholders::_1)));
}



void Cache::saveArtistsData(std::vector<std::unique_ptr<ArtistData>>& artistsData) {
    std::ofstream artistsDataStream{std::FSPATH(ARTISTS_DATA_PATH), std::ios::binary | std::ios::trunc };
    int count = artistsData.size();
    artistsDataStream.write(reinterpret_cast<char*>(&count), sizeof count);
    for (auto& artistData: artistsData) {
        std::string id = artistData->getId();
        int numberOfAlbums = artistData->getNumberOfAlbums();
        int numberOfTracks = artistData->getNumberOfTracks();

        auto& artist = artistData->getArtist();
        std::string name = artist.getName();

        writeString(artistsDataStream, id);
        artistsDataStream.write(reinterpret_cast<char*>(&numberOfAlbums), sizeof numberOfAlbums);
        artistsDataStream.write(reinterpret_cast<char*>(&numberOfTracks), sizeof numberOfTracks);
        writeString(artistsDataStream, name);
    }
    myNumberOfArtists = count;
    myArtistsSaved = true;
    updateLastUpdateInfo();
}



void Cache::saveAlbumsData(std::vector<std::unique_ptr<AlbumData>>& albumsData) {
    std::ofstream albumsDataStream{std::FSPATH(ALBUMS_DATA_PATH), std::ios::binary | std::ios::trunc };
    int count = albumsData.size();
    albumsDataStream.write(reinterpret_cast<char*>(&count), sizeof count);
    for (auto& albumData: albumsData) {
        std::string id = albumData->getId();
        std::string artUrl = albumData->getArtUrl();
        std::string artistId = albumData->getArtistId();
        int numberOfTracks = albumData->getNumberOfTracks();

        auto& album = albumData->getAlbum();
        std::string name = album.getName();
        int releaseYear = album.getReleaseYear();
        int mediaNumber = album.getMediaNumber();

        writeString(albumsDataStream, id);
        writeString(albumsDataStream, artUrl);
        writeString(albumsDataStream, artistId);
        albumsDataStream.write(reinterpret_cast<char*>(&numberOfTracks), sizeof numberOfTracks);
        writeString(albumsDataStream, name);
        albumsDataStream.write(reinterpret_cast<char*>(&releaseYear), sizeof releaseYear);
        albumsDataStream.write(reinterpret_cast<char*>(&mediaNumber), sizeof mediaNumber);
    }
    myNumberOfAlbums = count;
    myAlbumsSaved = true;
    updateLastUpdateInfo();
}



void Cache::saveTracksData(std::vector<std::unique_ptr<TrackData>>& tracksData) {
    std::ofstream tracksDataStream{std::FSPATH(TRACKS_DATA_PATH), std::ios::binary | std::ios::trunc };
    int count = tracksData.size();
    tracksDataStream.write(reinterpret_cast<char*>(&count), sizeof count);
    for (auto& trackData: tracksData) {
        std::string id = trackData->getId();
        std::string artistId = trackData->getArtistId();
        std::string albumId = trackData->getAlbumId();

        auto& track = trackData->getTrack();
        std::string name = track.getName();
        std::string disk = track.getDisk();
        int number = track.getNumber();
        std::string url = track.getUrl();

        writeString(tracksDataStream, id);
        writeString(tracksDataStream, artistId);
        writeString(tracksDataStream, albumId);
        writeString(tracksDataStream, name);
        writeString(tracksDataStream, disk);
        tracksDataStream.write(reinterpret_cast<char*>(&number), sizeof number);
        writeString(tracksDataStream, url);
    }
    myNumberOfTracks = count;
    myTracksSaved = true;
    updateLastUpdateInfo();
}



void Cache::updateAlbumArts(const std::map<std::string, QPixmap>& arts) const {
    for (auto idAndArt: arts) {
        idAndArt.second.save(QString::fromStdString(ALBUM_ARTS_DIR + idAndArt.first + ART_SUFFIX), "PNG");
    }
}



void Cache::onArtsLoadFinished() {
    LOG_DBG("Album art request has returned.");
    auto artsLoadFutureWatcher = reinterpret_cast<QFutureWatcher<std::pair<std::string, QImage>>*>(sender());
    artsLoadFutureWatcher->deleteLater();

    QFutureIterator<std::pair<std::string, QImage>> results{artsLoadFutureWatcher->future()};
    std::map<std::string, QPixmap> arts;
    while (results.hasNext()) {
        auto result = results.next();
        arts[result.first] = QPixmap::fromImage(result.second);
    }

    myRequestedAlbumArtIds.clear();
    readyAlbumArts(arts);
}



bool Cache::loadMeta(std::ifstream& metaStream) {
    int version = 0;
    metaStream.read(reinterpret_cast<char*>(&version), sizeof version);
    if (version != CACHE_VERSION) {
        return false;
    }

    myServerUrl = readString(metaStream);
    myUser = readString(metaStream);
    metaStream.read(reinterpret_cast<char*>(&myLastUpdate), sizeof myLastUpdate);
    metaStream.read(reinterpret_cast<char*>(&myNumberOfArtists), sizeof myNumberOfArtists);
    metaStream.read(reinterpret_cast<char*>(&myNumberOfAlbums), sizeof myNumberOfAlbums);
    metaStream.read(reinterpret_cast<char*>(&myNumberOfTracks), sizeof myNumberOfTracks);

    return true;
}



void Cache::saveMeta(std::chrono::system_clock::time_point lastUpdate) {
    std::ofstream metaStream{std::FSPATH(META_PATH), std::ios::binary | std::ios::trunc };
    int version = CACHE_VERSION;
    metaStream.write(reinterpret_cast<char*>(&version), sizeof version);
    writeString(metaStream, myServerUrl);
    writeString(metaStream, myUser);
    myLastUpdate = lastUpdate;
    metaStream.write(reinterpret_cast<char*>(&myLastUpdate), sizeof myLastUpdate);
    metaStream.write(reinterpret_cast<char*>(&myNumberOfArtists), sizeof myNumberOfArtists);
    metaStream.write(reinterpret_cast<char*>(&myNumberOfAlbums), sizeof myNumberOfAlbums);
    metaStream.write(reinterpret_cast<char*>(&myNumberOfTracks), sizeof myNumberOfTracks);
}



void Cache::invalidate() {
    Filesystem::removeAllFiles(ALBUM_ARTS_DIR);

    myServerUrl = myCurrentServerUrl;
    myUser = myCurrentUser;

    saveMeta(std::chrono::system_clock::time_point::min());
}



std::pair<std::string, QImage> Cache::loadAlbumArt(const std::string& id) const {
    QImage art;
    art.load(QString::fromStdString(ALBUM_ARTS_DIR + id + ART_SUFFIX), "PNG");
    return make_pair(id, art);
}



std::string Cache::readString(std::ifstream& stream) const {
    int length = 0;
    stream.read(reinterpret_cast<char*>(&length), sizeof length);
    std::string resultString;
    resultString.resize(length, ' ');
    char* begin = &*resultString.begin();
    stream.read(begin, length);
    return resultString;
}



void Cache::writeString(std::ofstream& stream, const std::string& str) const {
    int length = str.size();
    stream.write(reinterpret_cast<char*>(&length), sizeof length);
    stream << str;
}



void Cache::updateLastUpdateInfo() {
    if (myArtistsSaved + myAlbumsSaved + myTracksSaved == 1) {
        invalidate();
        myUpdateBegin = std::chrono::system_clock::now();
    } else if (myArtistsSaved + myAlbumsSaved + myTracksSaved == 3) {
        myArtistsSaved = false;
        myAlbumsSaved = false;
        myTracksSaved = false;
        saveMeta(myUpdateBegin);
    }
}

}
