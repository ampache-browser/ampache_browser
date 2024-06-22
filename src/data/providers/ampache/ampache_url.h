// ampache_url.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#ifndef AMPACHEURL_H
#define AMPACHEURL_H



#include <utility>
#include <string>



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
     * @brief Gets value of parameter 'id'.
     *
     * @return std::string
     */
    std::string parseIdValue() const;


    /**
     * @brief Gets value of parameter 'filter'.
     *
     * @return std::string
     */
    std::string parseFilterValue() const;

    /**
     * @brief Gets value of parameter 'action'.
     *
     * @return std::string
     */
    std::string parseActionValue() const;

    /**
     * @brief Replaces value of parameter 'ssid' with the given one.
     *
     * @param newSsidValue The new value of the parameter 'ssid'.
     * @return The url with the new 'ssid' value.
     */
    AmpacheUrl replaceSsidValue(const std::string& newSsidValue) const;

    /**
     * @brief Replaces value of parameter 'auth' with the given one.
     *
     * @param newAuthValue The new value of the parameter 'auth'.
     * @return The url with the new 'auth' value.
     */
    AmpacheUrl replaceAuthValue(const std::string& newAuthValue) const;

    /**
     * @brief Gets the string representation of the URL.
     *
     * @return std::string
     */
    std::string str() const;

private:
    // parameter names
    static const std::string PARAM_SSID;
    static const std::string PARAM_AUTH;
    static const std::string PARAM_ID;
    static const std::string PARAM_FILTER;
    static const std::string PARAM_ACTION;

    // arguments from the constructor
    const std::string myUrl;

    std::string parseValue(const std::string& parameterName) const;
    AmpacheUrl replaceValue(const std::string& parameterName, const std::string& newValue) const;
    std::pair<std::string::size_type, std::string::size_type> findParameterValue(const std::string& parameterName)
      const;
};

}



#endif // AMPACHEURL_H
