#ifndef IMAGE16_HPP
#define IMAGE16_HPP

#include <Magick++.h>
#include <vector>

#include "image.hpp"
#include "color.hpp"

class Image32Bpp;

/** 16 Bit Image BGR
  * Used for GBA mode 3.
  */
class Image16Bpp : public Image
{
    public:
        Image16Bpp(const Image32Bpp& image);
        void WriteData(std::ostream& file) const;
        void WriteCommonExport(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        Magick::Image ToMagick() const;
        std::vector<Color16> pixels;
};

#endif
