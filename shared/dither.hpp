#ifndef DITHER_HPP
#define DITHER_HPP

#include "color.hpp"
#include "reductionhelper.hpp"

// Implements algorithm from http://www.compuphase.com/riemer.htm
void RiemersmaDither(const Image16Bpp& inImage, Image8Bpp& outimage, unsigned short transparent, int dither, float ditherlevel);

#endif
