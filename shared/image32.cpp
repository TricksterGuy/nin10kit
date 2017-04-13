#include <cstdio>

#include "export_params.hpp"
#include "fileutils.hpp"
#include "logger.hpp"
#include "image32.hpp"
#include "magick_interface.hpp"
#include "shared.hpp"

unsigned int ArraySize3DS(unsigned int base_size, const std::string& pixel_format)
{
    if (pixel_format == "RGBA8")
    {
        return base_size * 4;
    }
    else if (pixel_format == "RGB8")
    {
        return base_size * 3;
    }
    else if (pixel_format == "RGB5A1" || pixel_format == "RGBA5551" || pixel_format == "RGB565" || pixel_format == "RGBA4")
    {
        return base_size;
    }
    FatalLog("Invalid pixel format %s given (this shouldn't happen)", pixel_format.c_str());
    return 0;
}

std::string ArrayDataType3DS(const std::string& pixel_format)
{
    if (pixel_format == "RGBA8" || pixel_format == "RGB8")
    {
        return "const unsigned char";
    }
    else if (pixel_format == "RGB5A1" || pixel_format == "RGBA5551" || pixel_format == "RGB565" || pixel_format == "RGBA4")
    {
        return "const unsigned short";
    }
    FatalLog("Invalid pixel format %s given (this shouldn't happen)", pixel_format.c_str());
    return "";
}

Image32Bpp::Image32Bpp(const Magick::Image& image, const std::string& name, const std::string& filename, unsigned int frame, bool animated) :
    Image(image.columns(), image.rows(), name, filename, frame, animated)
{
    CopyMagickPixels(image, pixels);
}

void Image32Bpp::WriteData(std::ostream& file) const
{
    char buffer[32];

    const std::string array_type = ArrayDataType3DS(params.mode);
    unsigned int array_size = ArraySize3DS(pixels.size(), params.mode);
    WriteBeginArray(file, array_type, export_name, "", array_size);

    if (params.mode == "RGBA8")
    {
        for (unsigned int i = 0; i < pixels.size(); i++)
        {
            const Color& color = pixels[i];
            snprintf(buffer, 32, "0x%02x,0x%02x,0x%02x,0x%02x", color.r, color.g, color.b, color.a);
            WriteElement(file, buffer, array_size, i, 8);
        }
    }
    else if (params.mode == "RGB8")
    {
        for (unsigned int i = 0; i < pixels.size(); i++)
        {
            const Color& color = pixels[i];
            snprintf(buffer, 32, "0x%02x,0x%02x,0x%02x", color.b, color.g, color.r);
            WriteElement(file, buffer, array_size, i, 12);
        }
    }
    else if (params.mode == "RGB5A1" || params.mode == "RGBA5551")
    {
        for (unsigned int i = 0; i < pixels.size(); i++)
        {
            const Color16 color(pixels[i]);
            snprintf(buffer, 32, "0x%04x", color.ToDSShort());
            WriteElement(file, buffer, array_size, i, 16);
        }
    }
    else if (params.mode == "RGB565")
    {
        for (unsigned int i = 0; i < pixels.size(); i++)
        {
            const Color& color = pixels[i];
            unsigned char r = color.r >> 3;
            unsigned char g = color.g >> 2;
            unsigned char b = color.b >> 3;
            unsigned short value = r | (g << 5) | (b << 11);
            snprintf(buffer, 32, "0x%04x", value);
            WriteElement(file, buffer, array_size, i, 16);
        }
    }
    else if (params.mode == "RGBA4")
    {
        for (unsigned int i = 0; i < pixels.size(); i++)
        {
            const Color& color = pixels[i];
            unsigned char r = color.r >> 4;
            unsigned char g = color.g >> 4;
            unsigned char b = color.b >> 4;
            unsigned char a = color.a >> 4;
            unsigned short value = r | (g << 4) | (b << 8) | (a << 12);
            snprintf(buffer, 32, "0x%04x", value);
            WriteElement(file, buffer, array_size, i, 16);
        }
    }

    WriteEndArray(file);
    WriteNewLine(file);
}

void Image32Bpp::WriteCommonExport(std::ostream& file) const
{
    unsigned int array_size = ArraySize3DS(pixels.size(), params.mode);
    WriteDefine(file, name, "_SIZE", array_size * 2);
    WriteDefine(file, name, "_LENGTH", array_size);
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
}

void Image32Bpp::WriteExport(std::ostream& file) const
{
    unsigned int array_size = ArraySize3DS(pixels.size(), params.mode);
    WriteExtern(file, "const unsigned char", export_name, "", array_size);
    if (!animated)
    {
        WriteDefine(file, export_name, "_SIZE", array_size * 2);
        WriteDefine(file, export_name, "_LENGTH", array_size);
        WriteDefine(file, export_name, "_WIDTH", width);
        WriteDefine(file, export_name, "_HEIGHT", height);
    }
    WriteNewLine(file);
}
