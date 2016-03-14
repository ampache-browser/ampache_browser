# Makefile (devel)
#
# Project: Ampache Browser
# License: GNU GPLv3
#
# Copyright (C) 2015 - 2016 Róbert Čerňanský



PLUGIN = ampache_browser${PLUGIN_SUFFIX}

SRCS = src/infrastructure/os_paths.cc \
       src/infrastructure/event/delegate_void.cc \
       src/infrastructure/event/event_void.cc \
       src/domain/album.cc \
       src/domain/track.cc \
       src/domain/artist.cc \
       src/data/data_objects/track_data.cc \
       src/data/data_objects/artist_data.cc \
       src/data/data_objects/album_data.cc \
       src/data/providers/ampache/ampache.cc \
       src/data/providers/ampache/ampache_url.cc \
       src/data/providers/ampache/scale_album_art_runnable.cc \
       src/data/providers/cache.cc \
       src/data/filters/artist_filter_for_albums.cc \
       src/data/filters/name_filter_for_albums.cc \
       src/data/filters/name_filter_for_artists.cc \
       src/data/filters/artist_filter_for_tracks.cc \
       src/data/filters/album_filter_for_tracks.cc \
       src/data/filters/name_filter_for_tracks.cc \
       src/data/repositories/track_repository.cc\
       src/data/repositories/artist_repository.cc \
       src/data/repositories/album_repository.cc \
       src/data/indices.cc \
       src/application/models/track_model.cc \
       src/application/models/artist_model.cc \
       src/application/models/request_groups.cc \
       src/application/models/requests.cc \
       src/application/models/request_group.cc \
       src/application/models/album_model.cc \
       src/application/data_loader.cc \
       src/application/ampache_browser.cc \
       src/ui/ui.cc \
       src/ui/ampache_browser_main_window.cc \
       src/ampache_browser_plugin.cc

MOC_SRCS = include/ui/moc_ui.cc \
           include/application/models/moc_album_model.cc \
           include/application/models/moc_artist_model.cc \
           include/application/models/moc_track_model.cc \
           include/data/providers/moc_ampache.cc \
           src/data/providers/ampache/moc_scale_album_art_runnable.cc \
           include/data/providers/moc_cache.cc \
           src/ui/moc_ampache_browser_main_window.cc

SRCS += ${MOC_SRCS}



CC = gcc
CXX = g++
AR = /usr/bin/ar
LD = ${CC}
CFLAGS = -g -ggdb -O0 -fno-inline-functions -std=gnu99 -pipe -Wall -Wextra -Wpedantic -Wtype-limits -fvisibility=hidden
CXXFLAGS = -g -ggdb -O0 -fno-inline-functions -std=c++11 -pipe -Wall -Wextra -Wpedantic -Wtype-limits \
  -Woverloaded-virtual -fvisibility=hidden -D PACKAGE=\"audacious-plugins\" \
  -D EXPORT="__attribute__((visibility(\"default\")))"
CPPFLAGS =  -I/home/hs/drawer/projects/audacious_plugins/AmpacheBrowser/elab/Audacious/audacious-test/install/include
LDFLAGS =  -Wl,-z,defs
LIBS = -lpthread -L/home/hs/drawer/projects/audacious_plugins/AmpacheBrowser/elab/Audacious/audacious-test/install/lib \
  -laudcore
PLUGIN_CFLAGS = -fPIC -DPIC
PLUGIN_LDFLAGS = -shared
PLUGIN_SUFFIX = .so

QT_CFLAGS ?= -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I/usr/include/qt5 -fPIC
QT_LIBS ?= -lQt5Widgets -lQt5Gui -lQt5Core
QTMULTIMEDIA_CFLAGS ?= -I/usr/include/qt5/QtNetwork -I/usr/include/qt5/QtGui \
  -I/usr/include/qt5/QtCore -I/usr/include/qt5  -fPIC
QTMULTIMEDIA_LIBS ?= -lQt5Network -lQt5Gui -lQt5Core

PLUGIN_OBJS = ${SRCS:.cc=.plugin.o}

.SUFFIXES:
.SUFFIXES: .cc .plugin.o
.PHONY: all

all:
	${MAKE} ${PLUGIN}

moc_%.cc:%.h
	moc $(DEFINES) $(INCPATH) $< -o $@

.cc.plugin.o:
	${CXX} ${PLUGIN_CFLAGS} ${CXXFLAGS} ${CPPFLAGS} -c -o $@ $<

${PLUGIN}: ${PLUGIN_OBJS}
	objs=""; \
	for i in ${PLUGIN_OBJS}; do \
		objs="$$objs $$i"; \
	done; \
	${LD} -o $@ $$objs ${PLUGIN_LDFLAGS} ${LDFLAGS} ${LIBS}

clean:
	for i in ${MOC_SRCS} ${PLUGIN_OBJS} ${PLUGIN}; do \
		if test -f $$i -o -d $$i; then \
			rm -fr $$i; \
		fi \
	done



plugindir := ${plugindir}/${GENERAL_PLUGIN_DIR}

LD = ${CXX}
CPPFLAGS += -I./include -I. ${QT_CFLAGS} ${QTMULTIMEDIA_CFLAGS} -I/usr/include/qt5/QtConcurrent
CFLAGS += ${PLUGIN_CFLAGS}
LIBS += ${QT_LIBS} ${QTMULTIMEDIA_LIBS} -lQt5Concurrent -laudqt
