#include <memory>
#include <vector>
#include <Magick++.h>

#include "export_params.hpp"
#include "imagedirect.hpp"

struct ImageInfo
{
    Magick::Image image;
    std::string name;
    bool hasPalette;
    size_t nrOfColors;
};

void DoDirectExport(const std::vector<std::pair<Magick::Image, std::string>>& images)
{
    if (images.size() > 0)
    {
        // check how many colors the images have and if they have a palette
        std::vector<ImageInfo> info16;
        std::vector<ImageInfo> info256;
        std::vector<ImageInfo> infoRGB;
        for (const auto &image : images)
        {
            ImageInfo info = {image.first, image.second, false, 0};
            info.hasPalette = image.first.classType() == MagickCore::ClassType::PseudoClass;
            info.nrOfColors = info.hasPalette ? image.first.colorMapSize() : 0;
            if (info.hasPalette && info.nrOfColors <= 16)
            {
                info16.push_back(info);
            }
            else if (info.hasPalette && info.nrOfColors <= 256)
            {
                info256.push_back(info);
            }
            else
            {
                infoRGB.push_back(info);
            }
        }
        // TODO: Find all images with common palettes and put them into one scene.
        //       Use export_params.symbol_base_name
        for (const ImageInfo & info : info16)
        {
            auto simg = std::make_shared<ImageDirect>(info.image, info.name);
            header.Add(simg);
            implementation.Add(simg);
        }
        for (const auto &info : info256)
        {
            auto simg = std::make_shared<ImageDirect>(info.image, info.name);
            header.Add(simg);
            implementation.Add(simg);
        }
        for (const auto &info : infoRGB)
        {
            auto simg = std::make_shared<ImageDirect>(info.image, info.name);
            header.Add(simg);
            implementation.Add(simg);
        }
    }
}
