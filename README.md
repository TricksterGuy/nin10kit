nin10kit
========

#Compiling from source via provided Makefiles
You can run sudo ./setup.sh which does the tasks in the list below.

1. Ensure you have a C++ compiler (sudo apt-get install build-essential)
2. Install dependency wxWidgets 3.0 (sudo apt-get install libwxgtk3.0-dev)
3. Install dependency Magick++ (sudo apt-get install libmagick++-dev libmagickcore-dev libmagickwand-dev)
4. Build the program (make)
5. Install the program (sudo make install)

#Installing via apt-get (for ubuntu versions trusty and beyond)
Execute these commands
* sudo add-apt-repository ppa:tricksterguy87/ppa-gt-cs2110
* sudo apt-get update
* sudo apt-get install nin10kit

#Compiling with CMake

To compile this program you will need the following things installed

1. Cmake (sudo apt-get install cmake-qt-gui)
2. wxWidgets 3.0 (sudo apt-get install libwxgtk3.0-dev)
3. ImageMagick (sudo apt-get install libmagick++-dev libmagickcore-dev libmagickwand-dev)

To set up the build environment run cmake-gui
"Where is the source code?" should be set to this directory containing CMakelists.txt
"Where to build the binaries?" can be set to any directory (preferable in a different directory than the above answer)
Hit Configure and choose which setup you want
Hit Generate and you should be good to go, execute make in the build directory.

