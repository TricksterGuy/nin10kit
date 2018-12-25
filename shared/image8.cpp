#include <cstdio>

#include "export_params.hpp"
#include "fileutils.hpp"
#include "logger.hpp"
#include "image8.hpp"
#include "image16.hpp"
#include "mediancut.hpp"
#include "shared.hpp"

Image8Bpp::Image8Bpp(const Image16Bpp& image, std::shared_ptr<Palette> global_palette) :
    Image(image), pixels(width * height), palette(global_palette), export_shared_info(global_palette == nullptr)
{
    // If the image width is odd error out
    if (width & 1 && !params.force)
        FatalLog("Image: %s width is not a multiple of 2. Found (%d, %d). Please fix. Use --force to override this.", name.c_str(), width, height);
    else if (width & 1 && params.force)
        WarnLog("Image: %s width is not a multiple of 2. Found (%d %d). Image data can't be written to the screen with DMA.", name.c_str(), width, height);

    if (!palette)
    {
        palette.reset(new Palette(export_name));
        GetPalette(image, params.palette_size, params.transparent_color, params.offset, *palette);
    }

    DitherAndReduceImage(image, params.transparent_color, params.dither, params.dither_level, params.offset, *this);
}

void Image8Bpp::WriteData(std::ostream& file) const
{
    // Sole owner of palette
    if (export_shared_info)
        palette->WriteData(file);
    WriteShortArray(file, export_name, "", pixels, 16);
    WriteNewLine(file);
}

void Image8Bpp::WriteCommonExport(std::ostream& file) const
{
    WriteDefine(file, name, "_SIZE", pixels.size());
    WriteDefine(file, name, "_LENGTH", pixels.size() / 2);
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
}

void Image8Bpp::WriteExport(std::ostream& file) const
{
    // Sole owner of palette
    if (export_shared_info)
        palette->WriteExport(file);
    WriteExtern(file, "const unsigned short", export_name, "", pixels.size() / 2);
    if (!animated)
    {
        WriteDefine(file, export_name, "_SIZE", pixels.size());
        WriteDefine(file, export_name, "_LENGTH", pixels.size() / 2);
        WriteDefine(file, export_name, "_WIDTH", width);
        WriteDefine(file, export_name, "_HEIGHT", height);
    }
    WriteNewLine(file);
}

Image8BppScene::Image8BppScene(const std::vector<Image16Bpp>& images16, const std::string& name, std::shared_ptr<Palette> global_palette) :
    Scene(name), palette(global_palette), export_shared_info(global_palette == nullptr)
{
    for (const auto& image : images16)
    {
        if (image.width & 1 && !params.force)
            FatalLog("Image: %s width is not a multiple of 2. Found (%d %d). Please fix.", name.c_str(), image.width, image.height);
        else if (image.width & 1 && params.force)
            WarnLog("Image: %s width is not a multiple of 2. Found (%d %d). Image data can't be written to the screen with DMA.", name.c_str(), image.width, image.height);
    }

    if (!palette)
    {
        palette.reset(new Palette(name));
        GetPalette(images16, params.palette_size, params.transparent_color, params.offset, *palette);
    }

    images.reserve(images16.size());
    for (const auto& image : images16)
        images.emplace_back(new Image8Bpp(image, palette));
}

const Image8Bpp& Image8BppScene::GetImage(int index) const
{
    const Image* image = images[index].get();
    const Image8Bpp* image8 = dynamic_cast<const Image8Bpp*>(image);
    if (!image8) FatalLog("Internal Error could not cast Image to Image8Bpp. This shouldn't happen");
    return *image8;
}

void Image8BppScene::WriteData(std::ostream& file) const
{
    if (export_shared_info)
        palette->WriteData(file);
    Scene::WriteData(file);
}

void Image8BppScene::WriteExport(std::ostream& file) const
{
    if (export_shared_info)
        palette->WriteExport(file);
    Scene::WriteExport(file);
}
