# nin10kit
(Formerly [brandontools](https://github.com/TricksterGuy/brandontools))

A set of tools for doing homebrew game development, includes look up table generators and and image to C exporter for Nintendo's Gameboy Advance, DS, and 3DS systems. In addition the GUI version of the program allows you to see what the image would look like on real hardware before exporting.

__Table of Contents__
* [Features](#features)
* [Installation](#installation)
  * [Ubuntu/Debian](#ubuntudebian-based-systems)
  * [Windows](#windows)
  * [macOS / OSX](#macos--osx)
* [Usage](#usage)
* [License](#license)

## Features

Methodology, the image export tool should be smart, there is a lot of artificial difficulty for new users to developing homebrew applications.  Things such as knowing the correct flags to set in the Graphics Registers, calculating tile id offsets, or whatever your data would overflow a section of memory should be the tools job.  This tool was developed out of my frustration of working with tiled backgrounds, sprites, palettes, and bitmaps.

* Support for practically any image format via ImageMagick.
* Supports resizing images before you export them.
* Support for animated image file types (or filetypes with multiple frames).  nin10kit is smart enough to detect an animated image and will also generate an array of pointers to each frame.
* Supports generating a palette (or palette banks) given any image. Can fine tune to generate less colors for palette for a given image.
* Supports passing in custom palettes with a set of images to use the color palette passed in.
* Supports setting a color to be transparent, and any pixels in the image that were originally transparent.
* Supports batch exporting of images, in the case of exporting tiles and/or a palette.  The tiles and palette will work for all images exported.
* Automatically can form 1D or 2D tile mapping for sprites.  Will calculate the tile ids for each sprite exported for you, along with the proper shape/size/palette bank id flags.
* Supports generating tiles from a set of background/map images. Along with the proper flags to pass the Background control registers.
* Supports "tilesets", that is an image with the tiles you will use in all maps and exporting against a set of background/map images. Easier to manipulate maps, since you know exactly what tile-id belongs to what tile.
* Automatically error if the amount of graphics data / tiles / sprites  would overflow their respective areas of memory. (For instance your image was composed of more than 1023 tiles).
* Supports generating look up tables for any function in math.h along with fixed point support.
* GUI version also can view what the images would look like on real hardware (pretty much).


## Installation
### Ubuntu/Debian Based Systems
#### Installing via apt-get

This set of instructions will only work on Ubuntu 16.04 and derivatives.

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

#### Compiling with Visual Studio Code / CMake

* **Must:** Install the "C/C++ extension" by Microsoft.
* **Recommended:** If you want intellisense functionality install the "C++ intellisense" extension by austin.
* **Must:** Install the "CMake Tools" extension by vector-of-bool.
* Restart / Reload Visual Studio Code if you have installed extensions.
* Open nin10kit folder using "Open folder...".
* Choose the proper active CMake kit if asked. On my system this is "GCC for x86_x64-linux-gnu ...".
* You should be able to build now using F7 and build + run / debug using F5. To run without debugging, use Shift+F5.

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
