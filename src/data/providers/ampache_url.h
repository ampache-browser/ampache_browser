// ampache_url.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef AMPACHEURL_H
#define AMPACHEURL_H



#include <string>
#include <sstream>



namespace data {

/**
 * @brief Ampache server URL.
 */
class AmpacheUrl {

public:
    /**
     * @brief Constructor
     *
     * @param url The URL string.
     */
    explicit AmpacheUrl(const std::string& url);

    /**
     * @brief Creates instance od AmpacheUrl representing URL od an album art.
     *
     * @param albumId ID of the album which art URL shall be created.
     * @param urlBase URL of Ampache server.
     * @param authToken Authentication token for communication with Ampache server.
     * @return data::AmpacheUrl
     */
    static AmpacheUrl createAlbumArtUrl(const std::string& albumId, const std::string& urlBase,
        const std::string& authToken);

    /**
     * @brief Gets value of parameter 'id'.
     *
     * @return std::string
     */
    std::string parseIdValue() const;

    /**
     * @brief Gets value of parameter 'action'.
     *
     * @return std::string
     */
    std::string parseActionValue() const;

    /**
     * @brief Gets the string representation of the URL.
     *
     * @return std::string
     */
    std::string str() const;

private:
    // parameter names
    static const std::string PARAM_AUTH;
    static const std::string PARAM_ID;
    static const std::string PARAM_ACTION;

    // arguments from the constructor
    const std::string myUrl;

    std::string parseValue(const std::string& parameterName) const;
};

}



#endif // AMPACHEURL_H
