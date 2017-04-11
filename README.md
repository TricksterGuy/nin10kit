nin10kit
========

# Ubuntu/Debian Based Systems
## Installing via apt-get

These instructions will only work on non EOL'd versions of ubuntu and derivatives starting with trusty.

Execute these commands

* sudo add-apt-repository ppa:tricksterguy87/ppa-gt-cs2110
* sudo apt-get update
* sudo apt-get install nin10kit

## Compiling with CMake

These instructions will work only on linux based systems, instructions assume a debian based system.
To compile this program you will need the following things installed

* Cmake (sudo apt-get install cmake)
* wxWidgets 3.0 (sudo apt-get install libwxgtk3.0-dev)
* ImageMagick (sudo apt-get install libmagick++-dev libmagickcore-dev libmagickwand-dev)

1. Create a build directory and go into that directory (mkdir build && cd build)
2. Generate a Makefile with cmake cmake ..
3. Then make to build the binaries
4. Then sudo make install to install the binaries

# Windows
## Using precompiled binaries.
Available in [Releases](https://github.com/TricksterGuy/nin10kit/releases).  Binaries are compiled in msys2+mingw and compiler gcc

## Compiling with CMake
Coming soon maybe?  Its kinda a bother though.

# MacOSX
## Installing via Homebrew
Contributed by [skyman](https://github.com/skyman)

brew install skyman/gt/nin10kit
