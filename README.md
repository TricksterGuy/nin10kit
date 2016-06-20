nin10kit
========

#Installing via apt-get (for ubuntu versions trusty and beyond)
Execute these commands
* sudo add-apt-repository ppa:tricksterguy87/ppa-gt-cs2110
* sudo apt-get update
* sudo apt-get install nin10kit

#Compiling with CMake

To compile this program you will need the following things installed

1. Cmake (sudo apt-get install cmake)
2. wxWidgets 3.0 (sudo apt-get install libwxgtk3.0-dev)
3. ImageMagick (sudo apt-get install libmagick++-dev libmagickcore-dev libmagickwand-dev)

1. Create a build directory and go into that directory (mkdir build && cd build)
2. Generate a Makefile with cmake cmake ..
3. Then make to build the binaries
4. Then sudo make install to install the binaries

