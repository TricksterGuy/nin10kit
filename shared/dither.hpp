#ifndef DITHER_HPP
#define DITHER_HPP

#include <iterator>
#include <memory>
#include <vector>

#include "color.hpp"
#include "reductionhelper.hpp"

void RiemersmaDither(const Image16Bpp& inImage, Image8Bpp& outimage, unsigned short transparent, int dither, float ditherlevel);

#endif
