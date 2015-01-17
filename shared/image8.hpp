#ifndef IMAGE8_HPP
#define IMAGE8_HPP

#include <Magick++.h>
#include <memory>
#include <vector>

#include "color.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "palette.hpp"

class Image16Bpp;

/** 8 Bit Image with palette
  * Used for GBA mode 4
  */
class Image8Bpp : public Image
{
    public:
        Image8Bpp(const Image16Bpp& image);
        Image8Bpp(const Image16Bpp& image, std::shared_ptr<Palette>& global_palette);
        void WriteData(std::ostream& file) const;
        void WriteCommonExport(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        virtual bool HasPalette() const {return true;}
        unsigned char At(int x, int y) const {return pixels[y * width + x];}
        Magick::Image ToMagick() const;
        std::vector<unsigned char> pixels;
        /** Palette this image uses */
        std::shared_ptr<Palette> palette;
    private:
        /** If true also export palette. */
        bool export_shared_info;
};

/** Represents a set of 8 bit images that share a palette
  * Used for GBA mode 4 in batch mode
  */
class Image8BppScene : public Scene
{
    public:
        Image8BppScene(const std::vector<Image16Bpp>& images, const std::string& name);
        const Image8Bpp& GetImage(int index) const;
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::shared_ptr<Palette> palette;
};

#endif
