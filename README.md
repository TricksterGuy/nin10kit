# nin10kit
(Formerly [brandontools](https://github.com/TricksterGuy/brandontools))

A set of tools for doing homebrew game development, includes look up table generators and and image to C exporter for Nintendo's Gameboy Advance, DS, and 3DS systems. In addition the GUI version of the program allows you to see what the image would look like on real hardware before exporting.

__Table of Contents__
* [Installation](#installation)
  * [Ubuntu/Debian](#ubuntudebian-based-systems)
  * [Windows](#windows)
  * [macOS / OSX](#macos--osx)
* [Usage](#usage)
* [License](#license)

## Installation
### Ubuntu/Debian Based Systems
#### Installing via apt-get

These instructions will only work on non EOL'd versions of Ubuntu and derivatives starting with trusty.

Execute these commands:
```bash
sudo add-apt-repository ppa:tricksterguy87/ppa-gt-cs2110
sudo apt-get update
sudo apt-get install nin10kit
```

#### Compiling with CMake

These instructions will work only on Linux based systems, instructions assume a Debian-based system.

To compile this program you will need the following things installed:
* CMake (`sudo apt-get install cmake`)
* wxWidgets 3.0 (`sudo apt-get install libwxgtk3.0-dev`)
* ImageMagick (`sudo apt-get install libmagick++-dev libmagickcore-dev libmagickwand-dev`)

Once you have that done, cd into the repository folder and follow these instructions:
1. Create a build directory and go into that directory: `mkdir build && cd build`
2. Generate a Makefile with cmake: `cmake ..`
3. Then run make to build the binaries: `make`
4. Then run make install to install the binaries: `sudo make install`

### Windows
#### Using precompiled binaries.
Available in [Releases](https://github.com/TricksterGuy/nin10kit/releases).  Binaries are compiled in msys2+mingw and compiler gcc.

#### Compiling with CMake
Coming soon maybe?  Its kinda a bother though.

### macOS / OSX
#### Installing via Homebrew
_Contributed by [skyman](https://github.com/skyman)._

Install homebrew if you haven't already:
```bash
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Install nin10kit:
```bash
brew install skyman/gt/nin10kit
```

#### Compiling with CMake
You need to install CMake, wxMac and ImageMagick 7 (doing this using Homebrew makes things much easier).

Once you have that done, cd into the repository folder and follow these instructions:
1. Create a build directory and go into that directory: `mkdir build && cd build`
2. Generate a Makefile with cmake: `cmake -DENABLE_MAGICK7_SUPPORT=ON ..`
3. Then run make to build the binaries: `make`
4. Then run make install to install the binaries: `sudo make install`

## Usage
Run the command `nin10kit` for help using the command line interface and `nin10kitgui` to launch the GUI application.

## License
This project is licensed under the Apache License. More information can be found in the [LICENSE](LICENSE) file.
