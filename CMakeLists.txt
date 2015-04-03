project(ampache_browser)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

# TODO: remove botan when on QT 5
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../include
    ${CMAKE_CURRENT_BINARY_DIR}/.. /usr/include/botan-1.10)

set(ampache_browser_SRCS src/application/requests.cpp src/application/request_group.cpp src/application/managed_ampache_service.cpp src/application/request_tracker.cpp src/application/album_model.cpp src/ui/ui.cpp src/application/ampache_browser.cpp src/application/ampache_service.cpp src/domain/album.cpp
    src/domain/track.cpp src/domain/artist.cpp src/ui/ampache_browser_main_window.cpp src/main.cpp)
# by HS
# headers with QT meta objects
set(ampache_browser_HEADERS_WITH_MO include/ui/ui.h include/application/album_model.h
    src/ui/ampache_browser_main_window.h src/application/ampache_service.h)
# qt4_automoc(${ampache_browser_SRCS})
qt4_wrap_cpp(ampache_browser_HEADERS_MOC ${ampache_browser_HEADERS_WITH_MO})
# add_executable(ampache_browser ${ampache_browser_SRCS})
add_executable(ampache_browser ${ampache_browser_SRCS} ${ampache_browser_HEADERS_MOC})

target_link_libraries(ampache_browser ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY}
    /usr/lib64/libbotan-1.10.so)
install(TARGETS ampache_browser RUNTIME DESTINATION bin)

add_subdirectory(src)

add_subdirectory(include)
