#include <cstdio>

#include "fileutils.hpp"
#include "logger.hpp"
#include "image32.hpp"

Image32Bpp::Image32Bpp(const Magick::Image& image, const std::string& name, const std::string& filename, unsigned int frame, bool animated) :
    Image(image.columns(), image.rows(), name, filename, frame, animated), pixels(width * height)
{
    unsigned int num_pixels = width * height;
    const Magick::PixelPacket* imageData = image.getConstPixels(0, 0, image.columns(), image.rows());

    size_t depth;
    MagickCore::GetMagickQuantumDepth(&depth);
    for (unsigned int i = 0; i < num_pixels; i++)
    {
        const Magick::PixelPacket& packet = imageData[i];
        unsigned char r, g, b, a;
        if (depth == 8)
        {
            r = packet.red;
            g = packet.green;
            b = packet.blue;
            a = packet.opacity;
        }
        else if (depth == 16)
        {
            r = (packet.red >> 8) & 0xFF;
            g = (packet.green >> 8) & 0xFF;
            b = (packet.blue >> 8) & 0xFF;
            a = (packet.opacity >> 8) * 0xFF;
        }
        else
        {
            // To get rid of warning
            b = g = r = a = 0;
            FatalLog("Image quantum not supported");
        }

        pixels[i] = Color(r, g, b, a);
    }
}

void Image32Bpp::WriteData(std::ostream& file) const
{
    char buffer[32];
    WriteBeginArray(file, "const unsigned char", export_name, "", pixels.size() * 3);
    for (unsigned int i = 0; i < pixels.size(); i++)
    {
        const Color& color = pixels[i];
        snprintf(buffer, 32, "0x%02x,0x%02x,0x%02x", color.b, color.g, color.r);
        WriteElement(file, buffer, pixels.size() * 3, i * 3, 24);
    }
    WriteEndArray(file);
    WriteNewLine(file);
}

void Image32Bpp::WriteCommonExport(std::ostream& file) const
{
    WriteDefine(file, name, "_SIZE", pixels.size());
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
}

void Image32Bpp::WriteExport(std::ostream& file) const
{
    WriteExtern(file, "const unsigned char", export_name, "", pixels.size());
    if (!animated)
    {
        WriteDefine(file, export_name, "_SIZE", pixels.size());
        WriteDefine(file, export_name, "_WIDTH", width);
        WriteDefine(file, export_name, "_HEIGHT", height);
    }
    WriteNewLine(file);
}
