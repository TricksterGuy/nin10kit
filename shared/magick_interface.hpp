#ifndef MAGICK_INTERFACE_HPP
#define MAGICK_INTERFACE_HPP

#include <Magick++.h>
#include <vector>
#include "alltypes.hpp"

void CopyMagickPixels(const Magick::Image& image, std::vector<Color>& out);
Magick::Image ToMagick(const Image32Bpp& image);
Magick::Image ToMagick(const Image16Bpp& image);
Magick::Image ToMagick(const Image8Bpp& image);
Magick::Image ToMagick(const Palette& palette);
Magick::Image ToMagick(const PaletteBankManager& paletteBanks);
Magick::Image ToMagick(const Sprite& sprite);
Magick::Image ToMagick(const Tileset& image);
Magick::Image ToMagick(const Map& map);

#endif
