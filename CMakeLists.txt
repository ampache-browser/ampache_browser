project(ampache_browser)
cmake_minimum_required(VERSION 2.8.11)
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5Xml)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../include ${CMAKE_CURRENT_BINARY_DIR}/..)

set(ampache_browser_SRCS src/data/track_data.cpp src/data/artist_data.cpp src/data/album_data.cpp src/data/ampache_service.cpp src/data/track_repository.cpp src/data/artist_repository.cpp
    src/data/album_repository.cpp src/application/models/track_model.cpp src/application/models/artist_model.cpp
    src/application/models/request_groups.cpp src/application/models/requests.cpp
    src/application/models/request_group.cpp src/application/models/album_model.cpp src/ui/ui.cpp
    src/application/ampache_browser.cpp src/domain/album.cpp src/domain/track.cpp src/domain/artist.cpp
    src/ui/ampache_browser_main_window.cpp src/main.cpp)

# headers with QT meta objects
set(ampache_browser_HEADERS_WITH_MO include/ui/ui.h include/application/models/album_model.h
    include/application/models/artist_model.h include/application/models/track_model.h
    src/ui/ampache_browser_main_window.h include/data/ampache_service.h)
qt5_wrap_cpp(ampache_browser_HEADERS_MOC ${ampache_browser_HEADERS_WITH_MO})

add_executable(ampache_browser ${ampache_browser_SRCS} ${ampache_browser_HEADERS_MOC})

target_link_libraries(ampache_browser Qt5::Core Qt5::Widgets Qt5::Network Qt5::Xml)

install(TARGETS ampache_browser RUNTIME DESTINATION bin)

add_subdirectory(src)

add_subdirectory(include)
