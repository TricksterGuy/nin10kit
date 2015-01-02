#include <cstdio>

#include "fileutils.hpp"
#include "logger.hpp"
#include "image8.hpp"
#include "image16.hpp"
#include "mediancut.hpp"
#include "shared.hpp"

Image8Bpp::Image8Bpp(const Image16Bpp& image) : Image(image), pixels(width * height), palette(new Palette(export_name)), export_shared_info(true)
{
    // If the image width is odd error out
    if (width & 1)
        FatalLog("Image: %s width is not a multiple of 2. Please fix", name.c_str());

    GetPalette(image.pixels, params.palette, params.transparent_color, params.offset, *palette);
    DitherAndReduceImage(image, params.transparent_color, params.dither, params.dither_level, params.offset, *this);
}

Image8Bpp::Image8Bpp(const Image16Bpp& image, std::shared_ptr<Palette>& global_palette) : Image(image), pixels(width * height), palette(global_palette), export_shared_info(false)
{
    if (width & 1)
        FatalLog("Image: %s width is not a multiple of 2. Please fix", name.c_str());

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
    WriteDefine(file, name, "_SIZE", pixels.size() / 2);
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
        WriteDefine(file, export_name, "_SIZE", pixels.size() / 2);
        WriteDefine(file, export_name, "_WIDTH", width);
        WriteDefine(file, export_name, "_HEIGHT", height);
    }
    WriteNewLine(file);
}

Magick::Image Image8Bpp::ToMagick() const
{
    Magick::Image ret(Magick::Geometry(width, height), Magick::Color(255, 0, 0));
    Magick::PixelPacket* packet = ret.getPixels(0, 0, width, height);
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            const Color color = palette->At(pixels[i * width + j]).ToColor();
            packet[i * width + j] = Magick::Color(color.r << 8, color.g << 8, color.b << 8);
        }
    }
    ret.syncPixels();
    ret.write("image8.png");
    return ret;
}

Image8BppScene::Image8BppScene(const std::vector<Image16Bpp>& images16, const std::string& name) : Scene(name), palette(new Palette(name))
{
    ///TODO finalize
    GetPalette(images16[0].pixels, 256, params.transparent_color, params.offset, *palette);

    images.reserve(images16.size());
    for (unsigned int i = 0; i < images16.size(); i++)
        images.emplace_back(new Image8Bpp(images16[i], palette));

    palette->ToMagick();
    images16[0].ToMagick();
    GetImage(0).ToMagick();

    unsigned long error_total = 0;
    for (const auto& color_ces : palette->colorEntryStats)
    {
        const auto& color = color_ces.first;
        const auto& ces = color_ces.second;
        VerboseLog("Color %d,%d,%d count %d index %d error %d importance %d", color.r, color.g, color.b, ces.count, ces.index, ces.error, ces.count * ces.error);
        error_total += ces.count * ces.error;
    }
    for (int index = 0; index < 256; index++)
    {
        const auto& pes = palette->paletteEntryStats[index];
        VerboseLog("Palette %d used %d perfect matched %d error_total %ld error/color %f", index, pes.used_count, pes.perfect_count, pes.error, pes.error / ((double)pes.used_count));
    }

    VerboseLog("Error total %ld Error per color %f Error per pixel %f", error_total, error_total * 1.0f / palette->colorEntryStats.size(),
               error_total * 1.0f / (images[0]->width * images[0]->height));
}

const Image8Bpp& Image8BppScene::GetImage(int index) const
{
    const Image* image = images[index].get();
    const Image8Bpp* image8 = dynamic_cast<const Image8Bpp*>(image);
    if (!image8) FatalLog("Could not cast Image to Image8Bpp. This shouldn't happen");
    return *image8;
}

void Image8BppScene::WriteData(std::ostream& file) const
{
    palette->WriteData(file);
    Scene::WriteData(file);
}

void Image8BppScene::WriteExport(std::ostream& file) const
{
    palette->WriteExport(file);
    Scene::WriteExport(file);
}
