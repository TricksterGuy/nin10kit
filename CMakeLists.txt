cmake_minimum_required (VERSION 2.6)
project (nin10kit)

EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )

set(CPACK_BINARY_DEB "ON")
set(CPACK_BINARY_RPM "ON")
set(CPACK_BINARY_TGZ "ON")
set(CPACK_BINARY_ZIP "ON")
set(CPACK_GENERATOR "DEB")
set(CPACK_NSIS_DISPLAY_NAME "nin10kit 1.0.0")
set(CPACK_NSIS_PACKAGE_NAME "nin10kit 1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tools for developing on Nintendo handheld systems")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "nin10kit 1.0.0")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "nin10kit 1.0.0")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_GENERATOR "")
set(CPACK_SOURCE_TGZ "ON")

set(CPACK_DEBIAN_PACKAGE_NAME "nin10kit")
set(CPACK_DEBIAN_PACKAGE_VERSION "1.0")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Brandon Whitehead <bwhitehead0308@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ".")

set(CPACK_PACKAGE_FILE_NAME "nin10kit-1.0.0-${ARCHITECTURE}")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${ARCHITECTURE})

include(CPack)
include(FindPkgConfig)

option(ENABLE_DEBUG "Build debug version" OFF)

IF(ENABLE_DEBUG)
    set(CMAKE_CXX_FLAGS "-g -pg -Wall -std=c++11")
ELSE()
    set(CMAKE_CXX_FLAGS "-O3 -Wall -std=c++11")
    set(CMAKE_EXE_LINKER_FLAGS "-s")
ENDIF(ENABLE_DEBUG)

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
