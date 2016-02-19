// ampache_url.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include "ampache_url.h"



using namespace std;



namespace data {

AmpacheUrl::AmpacheUrl(const string& url):
myUrl{url} {
}



AmpacheUrl AmpacheUrl::createAlbumArtUrl(const string& albumId, const string& urlBase, const string& authToken) {
    ostringstream urlStream;
    urlStream << urlBase << "image.php?id=" << albumId << "&" << PARAM_AUTH << "=" << authToken;
    return AmpacheUrl(urlStream.str());
}



string AmpacheUrl::parseIdValue() const {
    return parseValue(PARAM_ID);
}



string AmpacheUrl::parseActionValue() const {
    return parseValue(PARAM_ACTION);
}



string AmpacheUrl::str() const {
    return myUrl;
}



string AmpacheUrl::parseValue(const string& parameterName) const {
    auto paramsStart = myUrl.find("?");
    auto idStart = myUrl.find(parameterName + "=", paramsStart) + parameterName.length() + 1;
    auto idEnd = myUrl.find("&", idStart);
    return myUrl.substr(idStart, idEnd - idStart);
}



const string AmpacheUrl::PARAM_AUTH = "auth";
const string AmpacheUrl::PARAM_ID = "id";
const string AmpacheUrl::PARAM_ACTION = "action";

}
