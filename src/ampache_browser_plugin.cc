// ampache_browser_plugin.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#define AUD_PLUGIN_QT_ONLY
#include <libaudcore/audstrings.h>
#include <libaudcore/drct.h>
#include <libaudcore/i18n.h>
#include <libaudcore/interface.h>
#include <libaudcore/runtime.h>
#include <libaudcore/plugin.h>

#include "infrastructure/event/delegate.h"
#include "application/ampache_browser.h"



using namespace infrastructure;
using namespace ui;
using namespace application;



class AmpacheBrowserPlugin: public GeneralPlugin {

public:
    static const char about[];

    static const PluginInfo pluginInfo;

    AmpacheBrowserPlugin(): GeneralPlugin(pluginInfo, true) {
    }

    void cleanup() override;

    void* get_qt_widget() override;

private:
    std::unique_ptr<Ui> myUi = nullptr;
    std::unique_ptr<AmpacheBrowser> myAmpacheBrowser = nullptr;
};



const char AmpacheBrowserPlugin::about[] =
    N_("Ampache Browser\n\n"
        "Ampache client for Audacious.\n\n"
        "License: GNU GPLv3\n"
        "Copyright (C) Róbert Čerňanský\n");



const PluginInfo AmpacheBrowserPlugin::pluginInfo = {
    N_("Ampache Browser"),
    PACKAGE,
    about,
    nullptr
};



void AmpacheBrowserPlugin::cleanup() {
    // the termination may be executed asynchronously; normally handler of termination finished event would
    // destroy myUi and myAmpacheBrowser instances; it is however not a good idea since the objects would be
    // destroyed while handling the event
    myAmpacheBrowser->requestTermination();
}



void* AmpacheBrowserPlugin::get_qt_widget()
{
    auto ui = std::unique_ptr<Ui>{new Ui{}};
    myAmpacheBrowser = std::unique_ptr<AmpacheBrowser>{new AmpacheBrowser{*ui}};
    myUi = std::move(ui);
    return myUi->getMainWidget();
}



EXPORT AmpacheBrowserPlugin aud_plugin_instance;
