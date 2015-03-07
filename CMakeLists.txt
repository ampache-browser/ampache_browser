project(ampache_browser)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../include)

set(ampache_browser_SRCS src/ui/ampache_browser_main_window.cpp src/main.cpp)
# by HS
set(ampache_browser_HEADERS include/ui/ampache_browser_main_window.h)
# qt4_automoc(${ampache_browser_SRCS})
qt4_wrap_cpp(ampache_browser_HEADERS_MOC ${ampache_browser_HEADERS})
# add_executable(ampache_browser ${ampache_browser_SRCS})
add_executable(ampache_browser ${ampache_browser_SRCS} ${ampache_browser_HEADERS_MOC})

target_link_libraries(ampache_browser ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS ampache_browser RUNTIME DESTINATION bin)

add_subdirectory(src)

add_subdirectory(include)

add_subdirectory(include/ui)
