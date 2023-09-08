# CMakeLists.txt
#
# Project: Ampache Browser
# License: GNU GPLv3
#
# Copyright (C) 2015 - 2023 Róbert Čerňanský



cmake_minimum_required(VERSION 3.0)
project(ampache_browser VERSION 1.0.7)
set(VERSION_EXTRA "")
set(soVersion 0)

include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(FindIntl)

option(USE_QT6 "Use Qt6 instead of Qt5" ON)
if (${USE_QT6})
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent)
endif()

set(sources
    src/infrastructure/filesystem.cc
    src/infrastructure/os_paths.cc
    src/infrastructure/event/delegate_void.cc
    src/infrastructure/event/event_void.cc
    src/infrastructure/logging/logging.cc
    src/infrastructure/logging/qt_logger/set_qt_logger_default_message_pattern.cc
    src/infrastructure/logging/qt_logger/qt_logger.cc
    src/domain/album.cc
    src/domain/track.cc
    src/domain/artist.cc
    src/data/data_objects/track_data.cc
    src/data/data_objects/artist_data.cc
    src/data/data_objects/album_data.cc
    src/data/providers/connection_info.cc
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
    src/ui/custom_proxy_style.cc
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
    src/application/application.cc
    src/application/application_qt_internal.cc
    src/settings.cc
    src/ampache_browser.cc
    src/application_qt.cc)

# headers with QT meta objects
set(headersWithMo
    include/internal/application/models/artist_model.h
    include/internal/application/models/album_model.h
    include/internal/application/models/track_model.h
    src/data/providers/ampache/scale_album_art_runnable.h
    include/internal/data/providers/ampache.h
    include/internal/data/providers/cache.h
    src/ui/settings_dialog.h
    src/ui/ampache_browser_main_window.h
    include/internal/ui/ui.h)
qt_wrap_cpp(mocs ${headersWithMo})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_CXX_STANDARD 17)

option(USE_NLS "Build with native language support." ON)

add_subdirectory(src)
add_subdirectory(include)

set(libTarget ampache_browser)
set(libName ${libTarget}_${PROJECT_VERSION_MAJOR})

add_library(${libTarget} SHARED ${sources} ${mocs})

set_target_properties(${libTarget} PROPERTIES
    OUTPUT_NAME ${libName}
    VERSION ${soVersion}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
    SOVERSION ${soVersion})

target_include_directories(${libTarget}
    PRIVATE include/internal
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public> $<INSTALL_INTERFACE:include/${libName}>)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(${libTarget} PRIVATE -Wall -Wextra -Wpedantic -Wtype-limits -Woverloaded-virtual
        -Wformat-security)
endif()

target_compile_definitions(${libTarget}
    PRIVATE PACKAGE=\"${PROJECT_NAME}\"
    PRIVATE VERSION=\"${PROJECT_VERSION}$<$<BOOL:${VERSION_EXTRA}>:->${VERSION_EXTRA}\")
if (${USE_NLS})
    if (Intl_FOUND)
        target_compile_definitions(${libTarget} PRIVATE ENABLE_NLS)
    else()
        message(WARNING "Missing libintl. Native language support disabled.")
    endif()
endif()

target_link_libraries(${libTarget} PRIVATE ${Intl_LIBRARIES} Qt::Core Qt::Widgets Qt::Concurrent)

set(configFileName ${libName}-config.cmake)
write_basic_package_version_file(${libName}-config-version.cmake COMPATIBILITY SameMajorVersion)
configure_file(${libTarget}-config.cmake.cmakein ${configFileName} COPYONLY)
configure_file(${libTarget}.pc.cmakein ${libName}.pc @ONLY)
generate_export_header(${libTarget} EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/include/public/ampache_browser/export.h)

set(exportedLibTargets ${libName}-targets)
install(TARGETS ${libTarget} EXPORT ${exportedLibTargets}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/public/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${libName}
    FILES_MATCHING PATTERN "*.h"
    PATTERN CMakeFiles EXCLUDE)
install(FILES ${PROJECT_BINARY_DIR}/${libName}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(FILES ${PROJECT_BINARY_DIR}/${configFileName} ${PROJECT_BINARY_DIR}/${libName}-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${libName})

install(EXPORT ${exportedLibTargets} DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${libName})

# uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.cmakein"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
