# Ampache Browser

Ampache desktop client library.


## Description

**Ampache Browser** is a library that implements desktop client for [Ampache](http://ampache.org/).  It provides
end-user Qt UI and has a simple C++ interface that allows easy integration to a client application.


## Usage

Please see [Ampache Browser Audacious plugin](https://github.com/audacious-media-player/audacious-plugins/blob/master/src/ampache/ampache.cc)
for the reference usage.


## Installation

Download the latest version from the [releases page](https://github.com/ampache-browser/ampache_browser/releases/).
Additionally, following Qt 5 modules are required prior to the installation: Core, Widgets, Concurrent.  Finally, CMake
is required for building.

Unpack the installation archive and go to the unpacked directory:

    $ tar xf ampache_browser-<version>.tar.gz
    $ cd ampache_browser-<version>

Configure, build and install the library:

    $ cmake .
    $ make
    $ sudo make install


## Home

http://ampache-browser.org


## License

GNU GPLv3 (see COPYING for the license text).
