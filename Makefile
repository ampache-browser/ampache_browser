# Makefile
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
       src/data/providers/ampache/ampache_url.cc \
       src/data/providers/ampache/scale_album_art_runnable.cc \
       src/data/providers/ampache/ampache.cc \
       src/data/providers/cache.cc \
       src/data/indices.cc \
       src/data/filters/artist_filter_for_albums.cc \
       src/data/filters/artist_filter_for_tracks.cc \
       src/data/filters/album_filter_for_tracks.cc \
       src/data/filters/name_filter_for_artists.cc \
       src/data/filters/name_filter_for_albums.cc \
       src/data/filters/name_filter_for_tracks.cc \
       src/data/repositories/track_repository.cc \
       src/data/repositories/artist_repository.cc \
       src/data/repositories/album_repository.cc \
       src/ui/settings_dialog.cc \
       src/ui/ampache_browser_main_window.cc \
       src/ui/selected_items.cc \
       src/ui/ui.cc \
       src/application/models/request_group.cc \
       src/application/models/request_groups.cc \
       src/application/models/requests.cc \
       src/application/models/artist_model.cc \
       src/application/models/album_model.cc \
       src/application/models/track_model.cc \
       src/application/data_loader.cc \
       src/application/ampache_browser.cc \
       src/ampache_browser_plugin.cc

MOC_SRCS = include/application/models/moc_artist_model.cc \
           include/application/models/moc_album_model.cc \
           include/application/models/moc_track_model.cc \
           src/data/providers/ampache/moc_scale_album_art_runnable.cc \
           include/data/providers/moc_ampache.cc \
           include/data/providers/moc_cache.cc \
           src/ui/settings_dialog.cc \
           src/ui/moc_ampache_browser_main_window.cc \
           include/ui/moc_ui.cc

SRCS += ${MOC_SRCS}

moc_%.cc:%.h
	moc $(DEFINES) $(INCPATH) $< -o $@

include ../../buildsys.mk
include ../../extra.mk

plugindir := ${plugindir}/${GENERAL_PLUGIN_DIR}

LD = ${CXX}
CPPFLAGS += -I../.. -I./include -I. ${QT_CFLAGS} ${QTMULTIMEDIA_CFLAGS} -I/usr/include/qt5/QtConcurrent
CFLAGS += ${PLUGIN_CFLAGS}
LIBS += ${QT_LIBS} ${QTMULTIMEDIA_LIBS} -lQt5Concurrent

