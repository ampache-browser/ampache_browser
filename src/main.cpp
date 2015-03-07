// main.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QtGui/QApplication>
#include "ui/ampache_browser_main_window.h"



using namespace ui;



int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    AmpacheBrowserMainWindow ampacheBrowserMainWindow;
    ampacheBrowserMainWindow.show();
    return app.exec();
}
