// ampache_browser.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include "application/ampache_browser_app.h"
#include "ampache_browser/settings.h"
#include "ampache_browser/ampache_browser.h"

using namespace std;
using namespace application;



namespace ampache_browser {

AmpacheBrowser::AmpacheBrowser(Settings* const settings):
myAmpacheBrowserApp(new AmpacheBrowserApp{settings->mySettingsInternal.get()}) {
}



AmpacheBrowser::~AmpacheBrowser() {
}



void AmpacheBrowser::connectPlay(function<void(vector<string>)> callback) {
    myAmpacheBrowserApp->connectPlay(callback);
}



void AmpacheBrowser::connectCreatePlaylist(function<void(vector<string>)> callback) {
    myAmpacheBrowserApp->connectCreatePlaylist(callback);
}



void AmpacheBrowser::connectAddToPlaylist(function<void(vector<string>)> callback) {
    myAmpacheBrowserApp->connectAddToPlaylist(callback);
}



QWidget* AmpacheBrowser::getMainWidget() const {
    return myAmpacheBrowserApp->getMainWidget();
}



void AmpacheBrowser::requestTermination(function<void()> terminatedCb) {
    myAmpacheBrowserApp->requestTermination(terminatedCb);
}

}
