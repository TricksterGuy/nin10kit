#ifndef SHARED_HPP
#define SHARED_HPP

#include <string>
#include <vector>

#include <Magick++.h>

#include "headerfile.hpp"
#include "implementationfile.hpp"
#include "reductionhelper.hpp"

struct resize
{
    resize(int w = -1, int h = -1) : width(w), height(h) {}
    int width;
    int height;
    bool IsValid() const {return width > 0 && height > 0;}
};

struct ExportParams
{
    // General Export stuff
    std::string mode;
    std::string device;
    std::string output_dir;
    std::string export_file; // Export filename sans extension.
    std::string filename; // Full path to exported file.
    std::string symbol_base_name; // base name of generated symbols <sbn>_palette, <sbn>_map etc.

    std::vector<std::string> files;
    std::vector<std::string> tilesets;
    std::vector<Image32Bpp> images;
    std::vector<Image32Bpp> tileset_images;
    std::vector<std::string> names; // In batch names of the arrays for the images. if -names is given then it becomes those.

    // Optional stuff
    std::vector<resize> resizes;
    int transparent_color;

    // Palette options
    unsigned int offset;
    std::vector<int> weights;
    bool dither;
    float dither_level;
    unsigned int palette;
    bool split;
    int bpp;

    // Tile/map stuff
    int split_sbb;
    int border;
    bool force;
    bool reduce;

    // Sprite stuff
    bool for_bitmap;
    bool export_2d;

    std::vector<Image32Bpp> GetImages() const;
    std::vector<Image32Bpp> GetTilesets() const;
};

std::string ToUpper(const std::string& str);
std::string Chop(const std::string& filename);
std::string Sanitize(const std::string& filename);
std::string Format(const std::string& filename);
unsigned int log2(unsigned int x);

extern ExportParams params;

#endif
