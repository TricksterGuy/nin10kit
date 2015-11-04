nin10kit
========

#Compiling (Highly Recommended)
You can run sudo ./setup.sh which does the tasks in the list below.

1. Ensure you have a C++ compiler (sudo apt-get install build-essential)
2. Install dependency wxWidgets 3.0 (sudo apt-get install libwxgtk3.0-dev)
3. Install dependency Magick++ (sudo apt-get install libmagick++-dev libmagickcore-dev libmagickwand-dev)
4. Build the program (make)
5. Install the program (sudo make install)


#Installing from packages (For Ubuntu 14.10, 15.04+)

1. Under releases (>1.1) download nin10kit-15.04-* if you are on 15.04, the other one if you are on 14.10
2. Install dependencies (sudo apt-get install libwxgtk3.0-0 libmagick++-dev libmagickcore-dev libmagickwand-dev)
3. Install the package using dpkg (dpkg -i package_name)
4. If it fails for some reason uninstall the package (dpkg --purge nin10kit)


#Installing from packages (For Ubuntu 14.04 and earlier)

1. Under releases (version 1.0) download the appropriate package for your architecture (i386 for 32 bit amd64 for 64 bit)
2. Install dependencies (sudo apt-get install libwxgtk3.0-0 libmagick++5 libmagickcore5 libmagickwand5)
3. Install the package using dpkg (dpkg -i package_name)


#Compiling with CMake (Advanced)

To compile this program you will need the following things installed

1. Cmake (sudo apt-get install cmake-qt-gui)
2. wxWidgets 3.0 (sudo apt-get install libwxgtk3.0-dev)
3. ImageMagick (sudo apt-get install libmagick++-dev libmagickcore-dev libmagickwand-dev)

To set up the build environment run cmake-gui
"Where is the source code?" should be set to this directory containing CMakelists.txt
"Where to build the binaries?" can be set to any directory (preferable in a different directory than the above answer)
Hit Configure and choose which setup you want
Hit Generate and you should be good to go, execute make in the build directory.

