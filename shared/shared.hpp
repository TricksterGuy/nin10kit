#ifndef SHARED_HPP
#define SHARED_HPP

#include <set>
#include <string>
#include <vector>

#include <Magick++.h>

#include "color.hpp"
#include "headerfile.hpp"
#include "image32.hpp"
#include "implementationfile.hpp"
#include "lutgen.hpp"

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

    std::vector<LutSpecification> functions;
    std::vector<std::string> files;
    std::vector<std::string> tilesets;
    std::vector<std::string> palettes;
    std::vector<Image32Bpp> images;
    std::vector<Image32Bpp> tileset_images;
    std::vector<Image32Bpp> palette_images;
    std::vector<std::string> names; // In batch names of the arrays for the images. if -names is given then it becomes those.

    // Optional stuff
    std::vector<resize> resizes;
    bool transparent_given;
    Color transparent_color;
    bool export_images;

    // Palette options
    unsigned int offset;
    bool dither;
    float dither_level;
    unsigned int palette_size;
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

    // 3ds stuff
    bool rotate;
};

std::string ToUpper(const std::string& str);
std::string Chop(const std::string& filename);
std::string Sanitize(const std::string& filename);
std::string Format(const std::string& filename);
unsigned int log2(unsigned int x);

extern ExportParams params;
extern const std::set<std::string> valid_3ds_modes;
extern const std::set<std::string> valid_gba_modes;

#endif
