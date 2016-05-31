// connection_info.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H



#include <string>



namespace data {

class ConnectionInfo {

public:
    explicit ConnectionInfo(const std::string& serverUrl, const std::string& userName, const std::string& password,
        const std::string& proxyHost, const int proxyPort, const std::string& proxyUser,
        const std::string& proxyPassword);

    std::string getServerUrl() const;

    std::string getUserName() const;

    std::string getPasswordHash() const;

    std::string getProxyHost() const;

    int getProxyPort() const;

    std::string getProxyUser() const;

    std::string getProxyPassword() const;

    bool isProxyUsed() const;

    bool doesProxyRequireAuthentication() const;

private:
    const std::string myServerUrl;
    const std::string myUserName;
    const std::string myPasswordHash;
    const std::string myProxyHost;
    const int myProxyPort;
    const std::string myProxyUser;
    const std::string myProxyPassword;
};

}



#endif // CONNECTIONINFO_H
