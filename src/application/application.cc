// application.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include "application/application.h"


#include <memory>
#include "application/settings_internal.h"
#include "ampache_browser/settings.h"
#include "application/ampache_browser_app.h"
#include "ampache_browser/ampache_browser.h"
#include "application/qt_application_internal.h"

using namespace std;
using namespace ampache_browser;



namespace application {

Application::Application() {
    auto settingsInternal = new SettingsInternal{};
    mySettings = new Settings{unique_ptr<SettingsInternal>{settingsInternal}};
    myAmpacheBrowserApp = new AmpacheBrowserApp{*settingsInternal};
    myAmpacheBrowser = new AmpacheBrowser{unique_ptr<AmpacheBrowserApp>{myAmpacheBrowserApp}};
}



Application::~Application() {
}



unique_ptr<AmpacheBrowser> Application::getAmpacheBrowser() const {
    return unique_ptr<AmpacheBrowser>{myAmpacheBrowser};
}



unique_ptr<Settings> Application::getSettings() const {
    return unique_ptr<Settings>{mySettings};
}



AmpacheBrowserApp& Application::getAmpacheBrowserApp() const {
    return *myAmpacheBrowserApp;
}

}

