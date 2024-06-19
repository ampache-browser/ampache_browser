// ampache_browser.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "application/ampache_browser_app.h"
#include "ampache_browser/ampache_browser.h"

using namespace std;
using namespace application;



namespace ampache_browser {

AmpacheBrowser::~AmpacheBrowser() {
}



void AmpacheBrowser::connectPlay(function<void(const vector<string>&)> callback) {
    myAmpacheBrowserApp->connectPlay(callback);
}



void AmpacheBrowser::connectCreatePlaylist(function<void(const vector<string>&)> callback) {
    myAmpacheBrowserApp->connectCreatePlaylist(callback);
}



void AmpacheBrowser::connectAddToPlaylist(function<void(const vector<string>&)> callback) {
    myAmpacheBrowserApp->connectAddToPlaylist(callback);
}



AmpacheBrowser::AmpacheBrowser(unique_ptr<AmpacheBrowserApp> ampacheBrowserApp):
myAmpacheBrowserApp(move(ampacheBrowserApp)) {
}

}
