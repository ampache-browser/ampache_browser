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
    Ui* myUi = nullptr;
    AmpacheBrowser* myAmpacheBrowser = nullptr;

    void onTerminated();
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
    myAmpacheBrowser->terminated += DELEGATE0(&AmpacheBrowserPlugin::onTerminated);
    myAmpacheBrowser->requestTermination();
}



void* AmpacheBrowserPlugin::get_qt_widget()
{
    myUi = new Ui{};
    myAmpacheBrowser = new AmpacheBrowser{*myUi};
    return myUi->getMainWidget();
}



void AmpacheBrowserPlugin::onTerminated() {
    AUDINFO("Terminating.\n");
    myAmpacheBrowser->terminated -= DELEGATE0(&AmpacheBrowserPlugin::onTerminated);
    delete(myAmpacheBrowser);
    delete(myUi);
}



EXPORT AmpacheBrowserPlugin aud_plugin_instance;
