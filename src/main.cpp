// main.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QApplication>
#include "application/ampache_browser.h"



using namespace ui;
using namespace application;



int main(int argc, char** argv)
{
    auto application = new QApplication{argc, argv};
    auto ui = new Ui{};
    auto ampacheBrowser = new AmpacheBrowser{*ui};
    
    auto exit = application->exec();

    delete(ampacheBrowser);
    delete(ui);
    delete(application);

    return exit;
}
