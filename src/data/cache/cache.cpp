// cache.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <sys/stat.h>
#include <vector>
#include <memory>
#include <fstream>
#include <chrono>
#include "domain/artist.h"
#include "domain/album.h"
#include "domain/track.h"
#include "../artist_data.h"
#include "../album_data.h"
#include "../track_data.h"
#include "data/cache/cache.h"

using namespace std;
using namespace chrono;
using namespace domain;



namespace data {

/**
 * @warning Class expects that all save* methods will be called subsequently.
 */
Cache::Cache() {
    ifstream metaStream{META_PATH};
    if (!metaStream) {
        // TODO: Handle errors.
        struct stat cacheBaseStat;
        stat(CACHE_BASE_DIR.c_str(), &cacheBaseStat);
        if (!S_ISDIR(cacheBaseStat.st_mode)) {
            mkdir(CACHE_BASE_DIR.c_str(), 0700);
        }
        struct stat cacheStat;
        stat(CACHE_DIR.c_str(), &cacheStat);
        if (!S_ISDIR(cacheStat.st_mode)) {
            mkdir(CACHE_DIR.c_str(), 0777);
        }
        struct stat albumArtsStat;
        stat(ALBUM_ARTS_DIR.c_str(), &albumArtsStat);
        if (!S_ISDIR(albumArtsStat.st_mode)) {
            mkdir(ALBUM_ARTS_DIR.c_str(), 0777);
        }
        saveMeta(myLastUpdate);
    } else {
        int version = 0;
        metaStream.read(reinterpret_cast<char*>(&version), sizeof version);
        metaStream.read(reinterpret_cast<char*>(&myLastUpdate), sizeof myLastUpdate);
    }
}



system_clock::time_point Cache::getLastUpdate() const {
    return myLastUpdate;
}



vector<unique_ptr<ArtistData>> Cache::loadArtistsData() {
    vector<unique_ptr<ArtistData>> artistsData{};

    ifstream artistsDataStream{ARTISTS_DATA_PATH};
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



vector<unique_ptr<AlbumData>> Cache::loadAlbumsData() {
    vector<unique_ptr<AlbumData>> albumsData{};

    ifstream albumsDataStream{ALBUMS_DATA_PATH};
    int count = 0;
    albumsDataStream.read(reinterpret_cast<char*>(&count), sizeof count);
    for (int idx = 0; idx < count; idx++) {
        auto id = readString(albumsDataStream);
        auto artistId = readString(albumsDataStream);
        int numberOfTracks = 0;
        albumsDataStream.read(reinterpret_cast<char*>(&numberOfTracks), sizeof numberOfTracks);

        auto name = readString(albumsDataStream);
        int releaseYear = 0;
        albumsDataStream.read(reinterpret_cast<char*>(&releaseYear), sizeof releaseYear);

        albumsData.emplace_back(
            new AlbumData{id, "",  artistId, numberOfTracks, unique_ptr<Album>{new Album{id, name, releaseYear}}});
    }

    return albumsData;
}



vector<unique_ptr<TrackData>> Cache::loadTracksData() {
    vector<unique_ptr<TrackData>> tracksData{};

    ifstream tracksDataStream{TRACKS_DATA_PATH};
    int count = 0;
    tracksDataStream.read(reinterpret_cast<char*>(&count), sizeof count);
    for (int idx = 0; idx < count; idx++) {
        auto id = readString(tracksDataStream);
        auto artistId = readString(tracksDataStream);
        auto albumId = readString(tracksDataStream);

        auto name = readString(tracksDataStream);
        int number = 0;
        tracksDataStream.read(reinterpret_cast<char*>(&number), sizeof number);

        tracksData.emplace_back(
            new TrackData{id, artistId, albumId, unique_ptr<Track>{new Track{id, name, number, ""}}});
    }

    return tracksData;
}



map<string, QPixmap> Cache::loadAlbumArts(const vector<string>& ids) const {
    map<string, QPixmap> arts;
    for (string id: ids) {
        auto fileName = QString::fromStdString(ALBUM_ARTS_DIR + id + ART_SUFFIX);
        QPixmap art;
        art.load(fileName, "PNG");
        arts.emplace(id, art);
    }
    return arts;
}



void Cache::saveArtistsData(vector<unique_ptr<ArtistData>>& artistsData) {
    ofstream artistsDataStream{ARTISTS_DATA_PATH, ofstream::binary | ofstream::trunc};
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
    myArtistsSaved = true;
    updateLastUpdateInfo();
}



void Cache::saveAlbumsData(vector<unique_ptr<AlbumData>>& albumsData) {
    ofstream albumsDataStream{ALBUMS_DATA_PATH, ofstream::binary | ofstream::trunc};
    int count = albumsData.size();
    albumsDataStream.write(reinterpret_cast<char*>(&count), sizeof count);
    for (auto& albumData: albumsData) {
        string id = albumData->getId();
        string artistId = albumData->getArtistId();
        int numberOfTracks = albumData->getNumberOfTracks();

        auto& album = albumData->getAlbum();
        string name = album.getName();
        int releaseYear = album.getReleaseYear();

        writeString(albumsDataStream, id);
        writeString(albumsDataStream, artistId);
        albumsDataStream.write(reinterpret_cast<char*>(&numberOfTracks), sizeof numberOfTracks);
        writeString(albumsDataStream, name);
        albumsDataStream.write(reinterpret_cast<char*>(&releaseYear), sizeof releaseYear);
    }
    myAlbumsSaved = true;
    updateLastUpdateInfo();
}



void Cache::saveTracksData(vector<unique_ptr<TrackData>>& tracksData) {
    ofstream tracksDataStream{TRACKS_DATA_PATH, ofstream::binary | ofstream::trunc};
    int count = tracksData.size();
    tracksDataStream.write(reinterpret_cast<char*>(&count), sizeof count);
    for (auto& trackData: tracksData) {
        string id = trackData->getId();
        string artistId = trackData->getArtistId();
        string albumId = trackData->getAlbumId();

        auto& track = trackData->getTrack();
        string name = track.getName();
        int number = track.getNumber();

        writeString(tracksDataStream, id);
        writeString(tracksDataStream, artistId);
        writeString(tracksDataStream, albumId);
        writeString(tracksDataStream, name);
        tracksDataStream.write(reinterpret_cast<char*>(&number), sizeof number);
    }
    myTracksSaved = true;
    updateLastUpdateInfo();
}



void Cache::updateAlbumArts(const map<string, QPixmap>& arts) {
    for (auto idAndArt: arts) {
        auto fileName = QString::fromStdString(ALBUM_ARTS_DIR + idAndArt.first + ART_SUFFIX);
        idAndArt.second.save(fileName, "PNG");
    }
}



void Cache::saveMeta(system_clock::time_point lastUpdate) {
    ofstream metaStream{META_PATH, ofstream::binary | ofstream::trunc};
    int version = VERSION;
    metaStream.write(reinterpret_cast<char*>(&version), sizeof version);
    myLastUpdate = lastUpdate;
    metaStream.write(reinterpret_cast<char*>(&myLastUpdate), sizeof myLastUpdate);
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
        myUpdateBegin = system_clock::now();
    } else if (myArtistsSaved + myAlbumsSaved + myTracksSaved == 3) {
        myArtistsSaved = false;
        myAlbumsSaved = false;
        myTracksSaved = false;
        saveMeta(myUpdateBegin);
    }
}

}
