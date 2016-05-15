cmake_minimum_required(VERSION 3.0)
project(ampache_browser VERSION 0.0.0)

include(GNUInstallDirs)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(include/public include/internal)

configure_file(ampache_browser.pc.cmakein ampache_browser.pc @ONLY)

set(ampache_browser_SRCS
    src/infrastructure/os_paths.cc
    src/infrastructure/event/delegate_void.cc
    src/infrastructure/event/event_void.cc
    src/domain/album.cc
    src/domain/track.cc
    src/domain/artist.cc
    src/data/data_objects/track_data.cc
    src/data/data_objects/artist_data.cc
    src/data/data_objects/album_data.cc
    src/data/providers/ampache/ampache_url.cc
    src/data/providers/ampache/scale_album_art_runnable.cc
    src/data/providers/ampache/ampache.cc
    src/data/providers/cache.cc
    src/data/indices.cc
    src/data/filters/artist_filter_for_albums.cc
    src/data/filters/artist_filter_for_tracks.cc
    src/data/filters/album_filter_for_tracks.cc
    src/data/filters/name_filter_for_artists.cc
    src/data/filters/name_filter_for_albums.cc
    src/data/filters/name_filter_for_tracks.cc
    src/data/repositories/track_repository.cc
    src/data/repositories/artist_repository.cc
    src/data/repositories/album_repository.cc
    src/ui/settings_dialog.cc
    src/ui/ampache_browser_main_window.cc
    src/ui/selected_items.cc
    src/ui/ui.cc
    src/application/models/request_group.cc
    src/application/models/request_groups.cc
    src/application/models/requests.cc
    src/application/models/artist_model.cc
    src/application/models/album_model.cc
    src/application/models/track_model.cc
    src/application/data_loader.cc
    src/application/filtering.cc
    src/application/settings_internal.cc
    src/application/ampache_browser_app.cc
    src/ampache_browser/settings.cc
    src/ampache_browser/ampache_browser.cc)

# headers with QT meta objects
set(ampache_browser_HEADERS_WITH_MO
    include/internal/application/models/artist_model.h
    include/internal/application/models/album_model.h
    include/internal/application/models/track_model.h
    src/data/providers/ampache/scale_album_art_runnable.h
    include/internal/data/providers/ampache.h
    include/internal/data/providers/cache.h
    src/ui/settings_dialog.h
    src/ui/ampache_browser_main_window.h
    include/internal/ui/ui.h)
qt5_wrap_cpp(ampache_browser_HEADERS_MOC ${ampache_browser_HEADERS_WITH_MO})

#add_compile_options(-std=c++11 -Wall -Wextra -Wpedantic -Wtype-limits -Woverloaded-virtual -fvisibility=hidden)
add_compile_options(-std=c++11 -Wall -Wextra -Wpedantic -Wtype-limits -Woverloaded-virtual)
add_definitions(-DPACKAGE=\"audacious-plugins\" -DEXPORT=__attribute__\(\(visibility\(\"default\"\)\)\))

add_library(ampache_browser SHARED ${ampache_browser_SRCS} ${ampache_browser_HEADERS_MOC})

target_link_libraries(ampache_browser audcore Qt5::Core Qt5::Widgets Qt5Concurrent)

install(TARGETS ampache_browser LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_subdirectory(src)

add_subdirectory(include)
