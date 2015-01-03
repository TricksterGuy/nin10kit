#ifndef MEDIAN_CUT_HPP
#define MEDIAN_CUT_HPP

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "color.hpp"
#include "image16.hpp"
#include "image8.hpp"
#include "palette.hpp"

void GetPalette(const std::vector<Color16>& pixels, unsigned int num_colors, const Color16& transparent, unsigned int offset, Palette& palette);
void GetPalette(const Image16Bpp& image, unsigned int num_colors, const Color16& transparent, unsigned int offset, Palette& palette);
void GetPalette(const std::vector<Image16Bpp>& images, unsigned int num_colors, const Color16& transparent, unsigned int offset, Palette& palette);

void DitherAndReduceImage(const Image16Bpp& image, const Color16& transparent, bool dither, double dither_level, unsigned int offset, Image8Bpp& indexedImage);
void ReduceImage(const std::vector<Color16>& pixels, const Palette& palette, const Color16& transparent, unsigned int offset, std::vector<unsigned char>& indexedPixels);

#endif

