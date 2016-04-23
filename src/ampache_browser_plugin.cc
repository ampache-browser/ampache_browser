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
#include "ui/ui.h"
#include "application/ampache_browser.h"



using namespace infrastructure;
using namespace ui;
using namespace application;



class AmpacheBrowserPlugin: public GeneralPlugin {

public:
    static const char about[];

    static const PluginInfo pluginInfo;

    AmpacheBrowserPlugin(): GeneralPlugin(pluginInfo, false) {
    }

    void cleanup() override;

    void* get_qt_widget() override;

private:
    std::unique_ptr<Ui> myUi = nullptr;
    std::unique_ptr<AmpacheBrowser> myAmpacheBrowser = nullptr;

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
    myUi = std::unique_ptr<Ui>{new Ui{}};
    myAmpacheBrowser = std::unique_ptr<AmpacheBrowser>{new AmpacheBrowser{*myUi}};
    return myUi->getMainWidget();
}



void AmpacheBrowserPlugin::onTerminated() {
    AUDINFO("Terminating.\n");

    // instance of AmpacheBrowser is destroyed during handling of its event (together with other dependend objects);
    // the application must ensure that no instance variable of any object that is being destroyed here is accessed
    // after the handling of this event
    myAmpacheBrowser = nullptr;

    myUi = nullptr;
}



EXPORT AmpacheBrowserPlugin aud_plugin_instance;
