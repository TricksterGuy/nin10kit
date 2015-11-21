cmake_minimum_required (VERSION 2.6)
project (nin10kit)
include(FindPkgConfig)

option(ENABLE_DEBUG "Build debug version" OFF)

IF(ENABLE_DEBUG)
    set(CMAKE_CXX_FLAGS "-g -pg -Wall -std=c++11")
ELSE()
    set(CMAKE_CXX_FLAGS "-O3 -Wall -std=c++11")
    set(CMAKE_EXE_LINKER_FLAGS "-s")
ENDIF(ENABLE_DEBUG)
set(CMAKE_SKIP_RPATH TRUE)

# Source files definition
set(SRC_SHARED
    shared/3ds-exporter.cpp
    shared/ds-exporter.cpp
    shared/gba-exporter.cpp
    shared/cmd-line-parser-helper.cpp
    shared/color.cpp
    shared/cpercep.cpp
    shared/dither.cpp
    shared/exportfile.cpp
    shared/fileutils.cpp
    shared/headerfile.cpp
    shared/image.cpp
    shared/image16.cpp
    shared/image32.cpp
    shared/image8.cpp
    shared/implementationfile.cpp
    shared/logger.cpp
    shared/lut-exporter.cpp
    shared/lutgen.cpp
    shared/map.cpp
    shared/mediancut.cpp
    shared/palette.cpp
    shared/scanner.cpp
    shared/scene.cpp
    shared/shared.cpp
    shared/sprite.cpp
    shared/tile.cpp
    shared/tileset.cpp
)

set(SRC_NIN10KIT
    cli/main.cpp
)

set(SRC_NIN10KITGUI
    gui/nin10kit.cpp
    gui/nin10kitapp.cpp
    gui/nin10kitgui.cpp
    gui/nin10kitviewergui.cpp
    gui/nin10kitviewer.cpp
    gui/imageutil.cpp
    gui/exporter.cpp
    gui/wxlogger.cpp
)

find_package(wxWidgets REQUIRED core base)
include(${wxWidgets_USE_FILE})
find_package(ImageMagick COMPONENTS Magick++ MagickWand MagickCore REQUIRED)

include_directories(${nin10kit_SOURCE_DIR}/shared)
include_directories(${ImageMagick_INCLUDE_DIRS})
link_directories(${nin10kit_SOURCE_DIR}/shared)

add_definitions(-DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16)

add_library(
    shared_files
    STATIC
    ${SRC_SHARED}
)

target_link_libraries(
    shared_files
    ${ImageMagick_LIBRARIES}
)

add_executable(
    nin10kit
    ${SRC_NIN10KIT}
)

target_link_libraries(
    nin10kit
    m
    shared_files
    ${ImageMagick_LIBRARIES}
    ${wxWidgets_LIBRARIES}
)

add_executable(
    nin10kitgui
    ${SRC_NIN10KITGUI}
)

target_link_libraries(
    nin10kitgui
    m
    shared_files
    ${ImageMagick_LIBRARIES}    
    ${wxWidgets_LIBRARIES}
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/nin10kit DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/nin10kitgui DESTINATION bin)
install(FILES readme.pdf DESTINATION share/doc/nin10kit)
