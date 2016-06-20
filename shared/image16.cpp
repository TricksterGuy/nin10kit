#include <cstdio>

#include "logger.hpp"
#include "export_params.hpp"
#include "fileutils.hpp"
#include "image16.hpp"
#include "image32.hpp"
#include "shared.hpp"

Image16Bpp::Image16Bpp(const Image32Bpp& image) : Image(image), pixels(width * height)
{
    unsigned int num_pixels = width * height;
    const std::vector<Color>& pixels32 = image.pixels;

    for (unsigned int i = 0; i < num_pixels; i++)
        pixels[i] = Color16(pixels32[i]);
}

void Image16Bpp::WriteData(std::ostream& file) const
{
    WriteBeginArray(file, "const unsigned short", export_name, "", pixels.size());
    WriteColor16Array(file, pixels, 16, params.device == "GBA");
    WriteEndArray(file);
    WriteNewLine(file);
}

void Image16Bpp::WriteCommonExport(std::ostream& file) const
{
    WriteDefine(file, name, "_SIZE", pixels.size());
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
}

void Image16Bpp::WriteExport(std::ostream& file) const
{
    WriteExtern(file, "const unsigned short", export_name, "", pixels.size());
    if (!animated)
    {
        WriteDefine(file, export_name, "_SIZE", pixels.size());
        WriteDefine(file, export_name, "_WIDTH", width);
        WriteDefine(file, export_name, "_HEIGHT", height);
    }
    WriteNewLine(file);
}
