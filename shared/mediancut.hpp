#ifndef MEDIAN_CUT_HPP
#define MEDIAN_CUT_HPP

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "color.hpp"
#include "image16.hpp"
#include "image8.hpp"
#include "palette.hpp"

void GetPalette(const std::vector<Color16>& pixels, unsigned int num_colors, const Color& transparent, unsigned int offset, Palette& palette);
void QuantizeImage(const std::vector<Image16Bpp>& images, unsigned int num_colors, const Color& transparent, unsigned int offset, Palette& palette,
                   std::vector<unsigned char>& indexed);
void DitherAndReduceImage(const Image16Bpp& image, const Color& transparent, bool dither, double dither_level, unsigned int offset, Image8Bpp& indexedImage);

bool MedianCut(const std::vector<Color16>& image, unsigned int desiredColors, std::vector<Color16>& palette);

#endif

