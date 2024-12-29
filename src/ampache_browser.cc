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

using namespace application;



namespace ampache_browser {

AmpacheBrowser::~AmpacheBrowser() {
}



void AmpacheBrowser::connectPlay(std::function<void(const std::vector<std::string>&)> callback) {
    myAmpacheBrowserApp->connectPlay(callback);
}



void AmpacheBrowser::connectCreatePlaylist(std::function<void(const std::vector<std::string>&)> callback) {
    myAmpacheBrowserApp->connectCreatePlaylist(callback);
}



void AmpacheBrowser::connectAddToPlaylist(std::function<void(const std::vector<std::string>&)> callback) {
    myAmpacheBrowserApp->connectAddToPlaylist(callback);
}



AmpacheBrowser::AmpacheBrowser(std::unique_ptr<AmpacheBrowserApp> ampacheBrowserApp):
myAmpacheBrowserApp(std::move(ampacheBrowserApp)) {
}

}
