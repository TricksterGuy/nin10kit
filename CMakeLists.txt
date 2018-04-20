cmake_minimum_required (VERSION 2.6)
if(CMAKE_VERSION VERSION_EQUAL 3.0 OR CMAKE_VERSION VERSION_GREATER 3.0)
    # Ignore warnings about rpath behavior on OS X
    cmake_policy(SET CMP0042 NEW)
endif()
project (nin10kit)
include(FindPkgConfig)

option(ENABLE_MAGICK7_SUPPORT "Support ImageMagick7 API instead of ImageMagick6" OFF)

# Require C++11 build
if(NOT CMAKE_CXX_FLAGS MATCHES "-std=(c|gnu)\\+\\+11")
    message(STATUS "This project requires C++11. Adding -std=c++11 to CXXFLAGS.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

# Additional debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -pg -fprofile-arcs -ftest-coverage --coverage")
# Install directories
set(CMAKE_INSTALL_LIBDIR lib CACHE STRING "where to install libraries/plugins")
set(CMAKE_INSTALL_BINDIR bin CACHE STRING "where to install programs")

# Set default warning flags
set(PROJECT_WARNING_FLAGS "-Wall -Wno-deprecated -Wno-deprecated-declarations" CACHE STRING "Compiler warning flags to include")
mark_as_advanced(PROJECT_WARNING_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROJECT_WARNING_FLAGS}")

# Default to release build if not specified
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_EXE_LINKER_FLAGS "-s")
endif(CMAKE_BUILD_TYPE MATCHES "Release")

IF(ENABLE_MAGICK7_SUPPORT)
    add_definitions(-DMAGICK7_SUPPORT)
    add_definitions(-DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16)
ELSE()
    add_definitions(-DMAGICK6_SUPPORT)
    add_definitions(-DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16)
ENDIF(ENABLE_MAGICK7_SUPPORT)


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
    shared/magick_interface.cpp
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
    #gui/tilemap/nin10kitmap.cpp
    #gui/tilemap/nin10kitmapgui.cpp
    #gui/tilemap/mapcanvas.cpp
    #gui/tilemap/tilechooser.cpp
    gui/exporter.cpp
    gui/shared/colorutil.cpp
    gui/shared/imageutil.cpp
    gui/shared/nin10app.cpp
    gui/logging/wxlogger.cpp
)

set(SRC_NIN10EDIT
	gui/bitmap/nin10edit.cpp
	gui/bitmap/nin10editgui.cpp
	gui/bitmap/nin10editapp.cpp
	gui/bitmap/color_chooser_gui.cpp
	gui/bitmap/color_chooser_dialog.cpp
	gui/bitmap/color_info_panel.cpp
	gui/bitmap/palette_editor_panel.cpp
	gui/bitmap/graphics_editor_panel.cpp
    gui/exporter.cpp
    gui/shared/colorutil.cpp
    gui/shared/imageutil.cpp
    gui/shared/nin10app.cpp
    gui/logging/wxlogger.cpp
)

find_package(wxWidgets REQUIRED core base)
include(${wxWidgets_USE_FILE})

if(APPLE OR WIN32)
    message(STATUS "The platform appears to be macOS or windows, so using pkg-config to find ImageMagick.")
    pkg_check_modules(ImageMagick Magick++ MagickWand MagickCore)
else()
    find_package(ImageMagick COMPONENTS Magick++ MagickWand MagickCore REQUIRED)
endif(APPLE OR WIN32)

include_directories(${nin10kit_SOURCE_DIR}/shared)
include_directories(${nin10kit_SOURCE_DIR}/gui/logging)
include_directories(${nin10kit_SOURCE_DIR}/gui/shared)
include_directories(${ImageMagick_INCLUDE_DIRS})
link_directories(${nin10kit_SOURCE_DIR}/shared)

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

add_executable(
    nin10edit
    ${SRC_NIN10EDIT}
)

target_link_libraries(
    nin10edit
    m
    shared_files
    ${ImageMagick_LIBRARIES}
    ${wxWidgets_LIBRARIES}
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/nin10kit DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/nin10kitgui DESTINATION bin)
install(FILES readme.pdf DESTINATION share/doc/nin10kit)
