#ifndef IMAGE32_HPP
#define IMAGE32_HPP

#include <Magick++.h>
#include <vector>

#include "color.hpp"
#include "image.hpp"

/** Represents a 32 (or 24) Bits per pixel image
  * ImageMagick Images are converted to this class first
  * Used for 3ds exports.
  */
class Image32Bpp : public Image
{
    public:
        Image32Bpp(const Magick::Image& image, const std::string& name, const std::string& filename, unsigned int frame, bool animated);
        void WriteData(std::ostream& file) const;
        void WriteCommonExport(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        virtual std::string GetImageType() const {return "const unsigned char*";}
        std::vector<Color> pixels;
};

#endif
