// cache.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef CACHE_H
#define CACHE_H



#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include "infrastructure/os_paths.h"

class QPixmap;



namespace data {

class ArtistData;
class AlbumData;
class TrackData;



class Cache {

public:
    explicit Cache();

    std::chrono::system_clock::time_point getLastUpdate() const;

    std::vector<std::unique_ptr<ArtistData>> loadArtistsData();

    std::vector<std::unique_ptr<AlbumData>> loadAlbumsData();

    std::vector<std::unique_ptr<TrackData>> loadTracksData();

    std::map<std::string, QPixmap> loadAlbumArts(const std::vector<std::string>& ids) const;

    void saveArtistsData(std::vector<std::unique_ptr<ArtistData>>& artistsData);

    void saveAlbumsData(std::vector<std::unique_ptr<AlbumData>>& albumsData);

    void saveTracksData(std::vector<std::unique_ptr<TrackData>>& tracksData);

    void updateAlbumArts(const std::map<std::string, QPixmap>& arts);

private:
    int const VERSION = 0;
    const std::string CACHE_BASE_DIR = infrastructure::OsPaths::getCacheHome();
    const std::string CACHE_DIR = CACHE_BASE_DIR + "ampache_browser/";
    const std::string META_PATH = CACHE_DIR + "meta";
    const std::string ARTISTS_DATA_PATH = CACHE_DIR + "artists_data";
    const std::string ALBUMS_DATA_PATH = CACHE_DIR + "albums_data";
    const std::string TRACKS_DATA_PATH = CACHE_DIR + "tracks_data";
    const std::string ALBUM_ARTS_DIR = CACHE_DIR + "album_arts/";
    const std::string ART_SUFFIX = ".art";

    std::chrono::system_clock::time_point myLastUpdate = std::chrono::system_clock::time_point::min();
    std::chrono::system_clock::time_point myUpdateBegin = std::chrono::system_clock::time_point::min();
    bool myArtistsSaved = false;
    bool myAlbumsSaved = false;
    bool myTracksSaved = false;

    void saveMeta(std::chrono::system_clock::time_point lastUpdate);
    std::string readString(std::ifstream& stream) const;
    void writeString(std::ofstream& stream, const std::string& str) const;
    void updateLastUpdateInfo();
};

}



#endif // CACHE_H
