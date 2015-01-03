#ifndef DITHER_HPP
#define DITHER_HPP

#include "color.hpp"
#include "image16.hpp"
#include "image8.hpp"

// Implements algorithm from http://www.compuphase.com/riemer.htm
void RiemersmaDither(const Image16Bpp& inImage, Image8Bpp& outimage, const Color16& transparent, int dither, float ditherlevel);

#endif
