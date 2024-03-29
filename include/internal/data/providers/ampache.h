// ampache.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2018 Róbert Čerňanský



#ifndef AMPACHE_H
#define AMPACHE_H



#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <chrono>

#include <QObject>
#include <QPixmap>

#include "infrastructure/event/event.h"
#include "data/providers/connection_info.h"

class QXmlStreamReader;



namespace data {

class AlbumData;
class ArtistData;
class TrackData;
class ScaleAlbumArtRunnable;



// SMELL: It is not needed to expose entire service as public interface.  Only creation and initialization is needed in
// application level. - Split the interface?
/**
 * @brief Provides access to Ampache server.
 */
class Ampache: public QObject {
    Q_OBJECT

public:
    /**
     * @brief Callback function called when a network request finishes.
     */
    using NetworkRequestCb = std::function<void(const std::string& url, const char* content, int contentSize)>;

    /**
     * @brief Function for making a network request.
     */
    using NetworkRequestFn = std::function<void(const std::string& url, NetworkRequestCb& networkRequestCb)>;

    /**
     * @brief Constructor.
     *
     * @param connectionInfo Information used to connect to the Ampache server.
     * @param networkRequestFn Function that will be called to retrieve data from network.  Usage of this function
     *        is workaround for segfault on exit when QNetworkAccessManager is used together with Audacious.
     */
    explicit Ampache(const ConnectionInfo& connectionInfo, const NetworkRequestFn& networkRequestFn);

    Ampache(const Ampache& other) = delete;

    Ampache& operator=(const Ampache& other) = delete;

    /**
     * @brief Event fired when initial handshake with the server was successful.
     *
     * @note Most of the methods are usable only after this event is fired.
     *
     * @param error true if an error occured, false if the initialization was successful
     */
    infrastructure::Event<bool> initialized{};

    /**
     * @brief Event fired when some albums data has been retrieved from the server.
     *
     * @sa requestAlbums()
     */
    infrastructure::Event<std::pair<std::vector<std::unique_ptr<AlbumData>>, bool>> readyAlbums{};

    /**
     * @brief Event fired when some artists data has been retrieved from the server.
     *
     * @sa requestArtists()
     */
    infrastructure::Event<std::pair<std::vector<std::unique_ptr<ArtistData>>, bool>> readyArtists{};

    /**
     * @brief Event fired when some tracks data has been retrieved from the server.
     *
     * @sa requesTracks()
     */
    infrastructure::Event<std::pair<std::vector<std::unique_ptr<TrackData>>, bool>> readyTracks{};

    /**
     * @brief Event fired when some album arts has been retrieved from the server.
     *
     * @sa requestAlbumArts()
     */
    infrastructure::Event<std::map<std::string, QPixmap>> readyAlbumArts{};

    /**
     * @brief Event fired when the session was extended or new one created as as result of refreshSession() call.
     *
     * @sa refreshSession()
     */
    infrastructure::Event<bool> readySession{};

    /**
     * @brief Return the initialization status of the instance.
     *
     * @note This method can be called even if the ::initialized event was not fired.
     *
     * @return bool
     * @sa ::initialized
     */
    bool getIsInitialized() const;

    /**
     * @brief Gets the server URL.
     */
    std::string getUrl() const;

    /**
     * @brief Gets the user used to log in to the server.
     */
    std::string getUser() const;

    /**
     * @brief Gets time point of the latest database update as reported by the server during handshake.
     *
     * @return std::chrono::system_clock::time_point
     */
    std::chrono::system_clock::time_point getLastUpdate() const;

    /**
     * @brief Gets the number of albums as reported by the server during handshake.
     *
     * @return int
     */
    int numberOfAlbums() const;

    /**
     * @brief Gets the number of artists as reported by the server during handshake.
     *
     * @return int
     */
    int numberOfArtists() const;

    /**
     * @brief Gets the number of tracks as reported by the server during handshake.
     *
     * @return int
     */
    int numberOfTracks() const;

    /**
     * @brief Performs handshake with the server and obtains authentication token and basic data.
     *
     * @sa ::initialized
     */
    void initialize();

    /**
     * @brief Request album records from the server.
     *
     * @param offset Starting offset that shall be requested.
     * @param limit Maximal number of records that shall be requested.
     *
     * @sa ::readyAlbums
     */
    void requestAlbums(int offset, int limit);

    /**
     * @brief Request artist records from the server.
     *
     * @param offset Starting offset that shall be requested.
     * @param limit Maximal number of records that shall be requested.
     *
     * @sa ::readyArtists
     */
    void requestArtists(int offset, int limit);

    /**
     * @brief Request track records from the server.
     *
     * @param offset Starting offset that shall be requested.
     * @param limit Maximal number of records that shall be requested.
     *
     * @sa ::readyTracks
     */
    void requestTracks(int offset, int limit);

    /**
     * @brief Request album arts from the server.
     *
     * @note If this method is called before ::initialized event it immediately raises ::readyAlbumArts with
     * zero loaded arts.
     *
     * @param idsAndUrls Identifiers of the album art images that shall be requested paired with their URLs.  IDs are
     *        equal to album IDs.
     *
     * @sa ::readyAlbumArts
     */
    void requestAlbumArts(const std::map<std::string, std::string>& idsAndUrls);

    /**
     * @brief Extends the session or makes a new one if alread expired.
     *
     * @sa ::readySession
     */
    void refreshSession();

    /**
     * @brief Update authentication parameters which may have expired.
     *
     * The values are replaced with those obtained upon server connection.  This method does not extends/refreshes
     * the session with the server.
     *
     * @param url The URL string.
     * @return URL string with authentication values possibly replaced.
     *
     * @sa refreshSession()
     */
    std::string refreshUrl(const std::string& url) const;

private slots:
    void onScaleAlbumArtRunnableFinished(ScaleAlbumArtRunnable* scaleAlbumArtRunnable);

private:
     // Ampache server method names
    struct {
        const std::string Handshake = "handshake";
        const std::string Ping = "ping";
        const std::string Albums = "albums";
        const std::string Artists = "artists";
        const std::string Tracks = "songs";
    } Method;

    // arguments from the constructor
    const ConnectionInfo myConnectionInfo;
    const NetworkRequestFn myNetworkRequestFn;

    // network communication callback functions
    NetworkRequestCb myNetworkRequestCb;
    NetworkRequestCb myAlbumArtsNetworkRequestCb;

    // true if handshake with the server was successful
    bool myIsInitialized = false;

    // true if the session is currently being refreshed
    bool myIsRefreshingSession = false;

    // authentication token as returned by the server
    std::string myAuthToken = "";

    // basic properties as returned by the server during handshake
    std::chrono::system_clock::time_point myLastUpdate = std::chrono::system_clock::time_point::min();
    int myNumberOfAlbums = 0;
    int myNumberOfArtists = 0;
    int myNumberOfTracks = 0;

    // URLs of album arts which were requested to load and are currently pending to be loaded and/or scaled
    std::set<std::string> myPendingAlbumArts;

    // map of [URL, album art] of album arts that were requested to load and the request was fulfilled
    std::map<std::string, QPixmap> myFinishedAlbumArts;

    void onNetworkRequestFinished(const std::string& url, const char* content, int contentSize);
    void onAlbumArtsNetworkRequestFinished(const std::string& artUrl, const char* content, int contentSize);

    void connectToServer();
    void callMethod(const std::string& name, const std::map<std::string, std::string>& arguments);
    bool isError(QXmlStreamReader& xmlStreamReader);
    void dispatchToMethodHandler(const std::string& methodName, QXmlStreamReader& xmlStreamReader, bool error);
    void processHandshake(QXmlStreamReader& xmlStreamReader, bool error);
    void processPing(QXmlStreamReader& xmlStreamReader, bool error, bool isRetry);
    void readHandshakeData(QXmlStreamReader& xmlStreamReader);
    void processAlbums(QXmlStreamReader& xmlStreamReader, bool error);
    std::vector<std::unique_ptr<AlbumData>> createAlbums(QXmlStreamReader& xmlStreamReader) const;
    void processArtists(QXmlStreamReader& xmlStreamReader, bool error);
    std::vector<std::unique_ptr<ArtistData>> createArtists(QXmlStreamReader& xmlStreamReader) const;
    void processTracks(QXmlStreamReader& xmlStreamReader, bool error);
    std::vector<std::unique_ptr<TrackData>> createTracks(QXmlStreamReader& xmlStreamReader) const;
    void IfNoPendingClearFinishedAlbumArtsAndRaiseReady();
    std::string assembleUrlBase() const;
};

}



#endif // AMPACHE_H
