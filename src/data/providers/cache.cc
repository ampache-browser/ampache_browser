// cache.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <stdio.h>
#include <vector>
#include <memory>
#include <fstream>
#include <chrono>

#include <QString>
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

using namespace std;
using namespace chrono;
using namespace infrastructure;
using namespace domain;



namespace data {

/**
 * @warning Class expects that all save* methods will be called subsequently.
 */
Cache::Cache(const string& serverUrl, const string& user):
myCurrentServerUrl{serverUrl},
myCurrentUser{user} {
    if (!Filesystem::isDirExisting(ALBUM_ARTS_DIR)) {
        Filesystem::makePath(ALBUM_ARTS_DIR, 0700);
        // TODO: Handle errors.
    }
    ifstream metaStream{META_PATH};
    if (!metaStream) {
        invalidate();
    } else {
        loadMeta(metaStream);
    }
}



system_clock::time_point Cache::getLastUpdate() const {
    return myLastUpdate;
}



string Cache::getServerUrl() const {
    return myServerUrl;
}



string Cache::getUser() const {
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



vector<unique_ptr<ArtistData>> Cache::loadArtistsData() const {
    vector<unique_ptr<ArtistData>> artistsData{};

    ifstream artistsDataStream{ARTISTS_DATA_PATH, ios::binary};
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
            new ArtistData{id, numberOfAlbums, numberOfTracks, unique_ptr<Artist>{new Artist{id, name}}});
    }

    return artistsData;
}



vector<unique_ptr<AlbumData>> Cache::loadAlbumsData() const {
    vector<unique_ptr<AlbumData>> albumsData{};

    ifstream albumsDataStream{ALBUMS_DATA_PATH, ios::binary};
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
            new AlbumData{id, artUrl, artistId, numberOfTracks, unique_ptr<Album>{
                new Album{id, name, releaseYear, mediaNumber}}});
    }

    return albumsData;
}



vector<unique_ptr<TrackData>> Cache::loadTracksData() const {
    vector<unique_ptr<TrackData>> tracksData{};

    ifstream tracksDataStream{TRACKS_DATA_PATH, ios::binary};
    int count = 0;
    tracksDataStream.read(reinterpret_cast<char*>(&count), sizeof count);
    for (int idx = 0; idx < count; idx++) {
        auto id = readString(tracksDataStream);
        auto artistId = readString(tracksDataStream);
        auto albumId = readString(tracksDataStream);

        auto name = readString(tracksDataStream);
        int number = 0;
        tracksDataStream.read(reinterpret_cast<char*>(&number), sizeof number);
        auto url = readString(tracksDataStream);

        tracksData.emplace_back(
            new TrackData{id, artistId, albumId, unique_ptr<Track>{new Track{id, name, number, url}}});
    }

    return tracksData;
}



void Cache::requestAlbumArts(const vector<string>& ids) {
    LOG_DBG("Getting %d album arts.", ids.size());
    myRequestedAlbumArtIds = ids;
    auto artsLoadFutureWatcher = new QFutureWatcher<pair<string, QPixmap>>();
    connect(artsLoadFutureWatcher, SIGNAL(finished()), this, SLOT(onArtsLoadFinished()));
    artsLoadFutureWatcher->setFuture(QtConcurrent::mapped(myRequestedAlbumArtIds,
        bind(&Cache::loadAlbumArt, this, placeholders::_1)));
}



void Cache::saveArtistsData(vector<unique_ptr<ArtistData>>& artistsData) {
    ofstream artistsDataStream{ARTISTS_DATA_PATH, ios::binary | ios::trunc};
    int count = artistsData.size();
    artistsDataStream.write(reinterpret_cast<char*>(&count), sizeof count);
    for (auto& artistData: artistsData) {
        string id = artistData->getId();
        int numberOfAlbums = artistData->getNumberOfAlbums();
        int numberOfTracks = artistData->getNumberOfTracks();

        auto& artist = artistData->getArtist();
        string name = artist.getName();

        writeString(artistsDataStream, id);
        artistsDataStream.write(reinterpret_cast<char*>(&numberOfAlbums), sizeof numberOfAlbums);
        artistsDataStream.write(reinterpret_cast<char*>(&numberOfTracks), sizeof numberOfTracks);
        writeString(artistsDataStream, name);
    }
    myNumberOfArtists = count;
    myArtistsSaved = true;
    updateLastUpdateInfo();
}



void Cache::saveAlbumsData(vector<unique_ptr<AlbumData>>& albumsData) {
    ofstream albumsDataStream{ALBUMS_DATA_PATH, ios::binary | ios::trunc};
    int count = albumsData.size();
    albumsDataStream.write(reinterpret_cast<char*>(&count), sizeof count);
    for (auto& albumData: albumsData) {
        string id = albumData->getId();
        string artUrl = albumData->getArtUrl();
        string artistId = albumData->getArtistId();
        int numberOfTracks = albumData->getNumberOfTracks();

        auto& album = albumData->getAlbum();
        string name = album.getName();
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



void Cache::saveTracksData(vector<unique_ptr<TrackData>>& tracksData) {
    ofstream tracksDataStream{TRACKS_DATA_PATH, ios::binary | ios::trunc};
    int count = tracksData.size();
    tracksDataStream.write(reinterpret_cast<char*>(&count), sizeof count);
    for (auto& trackData: tracksData) {
        string id = trackData->getId();
        string artistId = trackData->getArtistId();
        string albumId = trackData->getAlbumId();

        auto& track = trackData->getTrack();
        string name = track.getName();
        int number = track.getNumber();
        string url = track.getUrl();

        writeString(tracksDataStream, id);
        writeString(tracksDataStream, artistId);
        writeString(tracksDataStream, albumId);
        writeString(tracksDataStream, name);
        tracksDataStream.write(reinterpret_cast<char*>(&number), sizeof number);
        writeString(tracksDataStream, url);
    }
    myNumberOfTracks = count;
    myTracksSaved = true;
    updateLastUpdateInfo();
}



void Cache::updateAlbumArts(const map<string, QPixmap>& arts) const {
    for (auto idAndArt: arts) {
        idAndArt.second.save(QString::fromStdString(ALBUM_ARTS_DIR + idAndArt.first + ART_SUFFIX), "PNG");
    }
}



void Cache::onArtsLoadFinished() {
    LOG_DBG("Album art request has returned.");
    auto artsLoadFutureWatcher = reinterpret_cast<QFutureWatcher<pair<string, QPixmap>>*>(sender());
    artsLoadFutureWatcher->deleteLater();

    QFutureIterator<pair<string, QPixmap>> results{artsLoadFutureWatcher->future()};
    map<string, QPixmap> arts;
    while (results.hasNext()) {
        auto result = results.next();
        arts[result.first] = result.second;
    }

    myRequestedAlbumArtIds.clear();
    readyAlbumArts(arts);
}



void Cache::loadMeta(ifstream& metaStream) {
    int version = 0;
    metaStream.read(reinterpret_cast<char*>(&version), sizeof version);
    myServerUrl = readString(metaStream);
    myUser = readString(metaStream);
    metaStream.read(reinterpret_cast<char*>(&myLastUpdate), sizeof myLastUpdate);
    metaStream.read(reinterpret_cast<char*>(&myNumberOfArtists), sizeof myNumberOfArtists);
    metaStream.read(reinterpret_cast<char*>(&myNumberOfAlbums), sizeof myNumberOfAlbums);
    metaStream.read(reinterpret_cast<char*>(&myNumberOfTracks), sizeof myNumberOfTracks);
}



void Cache::saveMeta(system_clock::time_point lastUpdate) {
    ofstream metaStream{META_PATH, ios::binary | ios::trunc};
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

    saveMeta(system_clock::time_point::min());
}



pair<string, QPixmap> Cache::loadAlbumArt(const string& id) const {
    QPixmap art;
    art.load(QString::fromStdString(ALBUM_ARTS_DIR + id + ART_SUFFIX), "PNG");
    return make_pair(id, art);
}



string Cache::readString(ifstream& stream) const {
    int length = 0;
    stream.read(reinterpret_cast<char*>(&length), sizeof length);
    string resultString;
    resultString.resize(length, ' ');
    char* begin = &*resultString.begin();
    stream.read(begin, length);
    return resultString;
}



void Cache::writeString(ofstream& stream, const string& str) const {
    int length = str.size();
    stream.write(reinterpret_cast<char*>(&length), sizeof length);
    stream << str;
}



void Cache::updateLastUpdateInfo() {
    if (myArtistsSaved + myAlbumsSaved + myTracksSaved == 1) {
        invalidate();
        myUpdateBegin = system_clock::now();
    } else if (myArtistsSaved + myAlbumsSaved + myTracksSaved == 3) {
        myArtistsSaved = false;
        myAlbumsSaved = false;
        myTracksSaved = false;
        saveMeta(myUpdateBegin);
    }
}

}
