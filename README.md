# Ampache Browser

Ampache desktop client library.


## Description

**Ampache Browser** is a library that implements desktop client for [Ampache](http://ampache.org/).  It provides
end-user Qt UI and has a simple C++ interface that allows easy integration to a client application.


## Usage

Please see [Ampache Browser Audacious plugin](https://github.com/ampache-browser/audacious_plugin/blob/master/src/ampache_browser/ampache_browser_plugin.cc)
for the reference usage.


## Installation

Following Qt 5 modules are required prior to the installation: Core, Widgets, Concurrent.  CMake is required for
building.  When all dependencies are installed type to the console:

    $ git clone https://github.com/ampache-browser/ampache_browser.git
    $ cd ampache_browser
    $ cmake .
    $ make
    $ sudo make install


## Home

http://ampache-browser.org


## License

GNU GPLv3 (see COPYING for the license text).
