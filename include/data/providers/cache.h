// cache.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef CACHE_H
#define CACHE_H



#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <QObject>
#include <QPixmap>

#include "infrastructure/event/event.h"
#include "infrastructure/os_paths.h"



namespace data {

class ArtistData;
class AlbumData;
class TrackData;



/**
 * @brief Provides access to the disk cache.
 */
class Cache: public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     *
     * @param serverUrl URL of the Ampache server which data shall be cached.
     * @param user Ampache server user whose data shall be cached.
     */
    explicit Cache(const std::string& serverUrl, const std::string& user);

    /**
     * @brief Event fired when some album arts has been retrieved from disk.
     *
     * @sa requestAlbumArts()
     */
    infrastructure::Event<std::map<std::string, QPixmap>> readyAlbumArts{};

    /**
     * @brief Gets URL of the Ampache server which data are cached.
     */
    std::string getServerUrl() const;

    /**
     * @brief Gets the Ampache server user whose data are cached.
     */
    std::string getUser() const;

    /**
     * @brief Gets time point of the latest cache update.
     *
     * If this time point is earlier that the one read directly from Ampache server then the cached data should be
     * treated as obsolete.
     *
     * @return Time point of lastest update or std::chrono::system_clock::time_point::min() if cache data are not
     *         available.
     */
    std::chrono::system_clock::time_point getLastUpdate() const;

    /**
     * @brief Gets the number of cached albums.
     *
     * @return int
     */
    int numberOfAlbums() const;

    /**
     * @brief Gets the number of cached artists.
     *
     * @return int
     */
    int numberOfArtists() const;

    /**
     * @brief Gets the number of cached tracks.
     *
     * @return int
     */
    int numberOfTracks() const;

    /**
     * @brief Load artist records from the disk.
     */
    std::vector<std::unique_ptr<ArtistData>> loadArtistsData() const;

    /**
     * @brief Load album records from the disk.
     */
    std::vector<std::unique_ptr<AlbumData>> loadAlbumsData() const;

    /**
     * @brief Load track records from the disk.
     */
    std::vector<std::unique_ptr<TrackData>> loadTracksData() const;

    /**
     * @brief Request album arts from disk.
     *
     * @param ids Identifiers of album arts that shall be loaded as specified during saving.
     *
     * @sa ::readyAlbumArts, updateAlbumArts()
     */
    void requestAlbumArts(const std::vector<std::string>& ids);

    /**
     * @brief Store artists data to disk.
     *
     * @note All previously stored artist data will be removed.
     *
     * @param artistsData The data which shall be saved.
     */
    void saveArtistsData(std::vector<std::unique_ptr<ArtistData>>& artistsData);

    /**
     * @brief Store albums data to disk.
     *
     * @note All previously stored album data will be removed.
     *
     * @param albumsData The data which shall be saved.
     */
    void saveAlbumsData(std::vector<std::unique_ptr<AlbumData>>& albumsData);

    /**
     * @brief Store tracks data to disk.
     *
     * @note All previously stored track data will be removed.
     *
     * @param tracksData The data which shall be saved.
     */
    void saveTracksData(std::vector<std::unique_ptr<TrackData>>& tracksData);

    /**
     * @brief Store album arts to disk.
     *
     * @param arts Map of [identifier, album art] that shall be saved.
     */
    void updateAlbumArts(const std::map<std::string, QPixmap>& arts) const;

private slots:
    void onArtsLoadFinished();

private:
    // cache format version
    int const CACHE_VERSION = 0;

    // SMELL: Use audacious cache dir.?
    // user cache directory
    const std::string CACHE_BASE_DIR = infrastructure::OsPaths::getCacheHome();

    // cache directory of Ampache Browser
    const std::string CACHE_DIR = CACHE_BASE_DIR + "ampache_browser/";

    // path to cache meta file
    const std::string META_PATH = CACHE_DIR + "meta";

    // path to artists data cache file
    const std::string ARTISTS_DATA_PATH = CACHE_DIR + "artists_data";

    // path to albums data cache file
    const std::string ALBUMS_DATA_PATH = CACHE_DIR + "albums_data";

    // path to tracks data cache file
    const std::string TRACKS_DATA_PATH = CACHE_DIR + "tracks_data";

    // album arts cache directory
    const std::string ALBUM_ARTS_DIR = CACHE_DIR + "album_arts/";

    // suffix of cached album art file
    const std::string ART_SUFFIX = ".art";

    // server URL and user name that is currently used to connect to the actual server
    std::string myCurrentServerUrl = "";
    std::string myCurrentUser = "";

    // server URL and user name of the cached data
    std::string myServerUrl = "";
    std::string myUser = "";

    // time point of the latest cache update
    std::chrono::system_clock::time_point myLastUpdate = std::chrono::system_clock::time_point::min();

    // time point when the cache update begun
    std::chrono::system_clock::time_point myUpdateBegin = std::chrono::system_clock::time_point::min();

    // basic properties read from meta file
    int myNumberOfAlbums = 0;
    int myNumberOfArtists = 0;
    int myNumberOfTracks = 0;

    // save status of particular data
    bool myArtistsSaved = false;
    bool myAlbumsSaved = false;
    bool myTracksSaved = false;

    // identifiers of requested album arts which was not loaded yet
    std::vector<std::string> myRequestedAlbumArtIds;

    void loadMeta(std::ifstream& metaStream);
    void saveMeta(std::chrono::system_clock::time_point lastUpdate);
    void invalidate();
    std::pair<std::string, QPixmap> loadAlbumArt(const std::string& id) const;
    std::string readString(std::ifstream& stream) const;
    void writeString(std::ofstream& stream, const std::string& str) const;
    void updateLastUpdateInfo();
};

}



#endif // CACHE_H
