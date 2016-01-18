// ampache_browser_plugin.cpp
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
#include <libaudcore/plugin.h>

#include "application/ampache_browser.h"



using namespace ui;
using namespace application;



class AmpacheBrowserPlugin: public GeneralPlugin {

public:
    static const char about[];

    static constexpr PluginInfo info = {
        N_("Ampache Browser"),
        PACKAGE,
        about,
        nullptr
    };

    constexpr AmpacheBrowserPlugin(): GeneralPlugin(info, true) {
    }

    void* get_qt_widget();
};



EXPORT AmpacheBrowserPlugin aud_plugin_instance;



const char AmpacheBrowserPlugin::about[] =
    N_("Ampache Browser\n\n"
        "Ampache client for Audacious.\n\n"
        "License: GNU GPLv3\n"
        "Copyright (C) Róbert Čerňanský\n");



void* AmpacheBrowserPlugin::get_qt_widget()
{
    auto ui = new Ui{};
    // SMELL: The AmpacheBrowser instance is not owned and not released.
    new AmpacheBrowser{*ui};

    return ui->getMainWidget();
}
