#include "imagedirect.hpp"

#include "export_params.hpp"
#include "fileutils.hpp"
#include "logger.hpp"
#include "shared.hpp"

#include <cstdio>

std::shared_ptr<Palette> GetImagePalette(const Magick::Image &image, const std::string &name)
{
    std::vector<Color16> colors;
    colors.resize(image.colorMapSize());
    if (image.colorMapSize() <= 256)
    {
        for (size_t i = 0; i < image.colorMapSize(); ++i)
        {
            Magick::Color mc = image.colorMap(i);
            unsigned char r = MagickCore::ScaleQuantumToChar(mc.redQuantum());
            unsigned char g = MagickCore::ScaleQuantumToChar(mc.greenQuantum());
            unsigned char b = MagickCore::ScaleQuantumToChar(mc.blueQuantum());
            unsigned char a = MagickCore::ScaleQuantumToChar(mc.alphaQuantum());
            colors[i] = Color16(r, g, b, a);
        }
        return std::make_shared<Palette>(colors, name);
    }
    return std::shared_ptr<Palette>();
}

ImageDirect::ImageDirect(const Magick::Image &image, const std::string& name, std::shared_ptr<Palette> globalPalette) 
    : Image(image.columns(), image.rows(), name)
    , m_imageTypeString("const unsigned short")
    , m_nrOfPixels(image.columns() * image.rows())
    , m_palette(globalPalette)
{
    // check if paletted or direct color
    if (image.classType() == MagickCore::ClassType::PseudoClass)
    {
        // paletted. check if we have one, or get it from the image
        if (!m_palette)
        {
            m_palette = GetImagePalette(image, name);
        }
        // convert pixel data
        if (m_palette->Size() <= 16)
        {
            m_nrOfuint16Pixels = m_nrOfPixels / 4;
            if (width & 3)
            {
                WarnLog("Image: %s width is not a multiple of 4. Found (%d %d). Image data can't be written to the screen with DMA.", name.c_str(), width, height);
            }
            // reserve data half the number of pixels, we fit 2*4bpp pixels into one byte
            m_data.resize(m_nrOfPixels / 2);
            const Magick::PixelPacket *pixelData = image.getConstPixels(0, 0, width, height);
            for (unsigned int i = 0; i < (m_nrOfPixels / 2); ++i)
            {
                uint8_t p0 = pixelData[i*2].blue & 0x0F;
                uint8_t p1 = pixelData[i*2+1].blue & 0x0F;
                m_data[i] = (p1 << 4) | p0;
            }
        }
        else if (m_palette->Size() <= 256)
        {
            m_nrOfuint16Pixels = m_nrOfPixels / 2;
            if (width & 1)
            {
                WarnLog("Image: %s width is not a multiple of 2. Found (%d %d). Image data can't be written to the screen with DMA.", name.c_str(), width, height);
            }
            // reserve data = number of pixels
            m_data.resize(m_nrOfPixels);
            const Magick::PixelPacket *pixelData = image.getConstPixels(0, 0, width, height);
            for (unsigned int i = 0; i < (m_nrOfPixels); ++i)
            {
                m_data[i] = pixelData[i].blue & 0xFF;
            }
        }
    }
    // we have to convert everything above 8bit, because ImageMagick is oh so convenient
    // and hides the image data from us. we just get 16bit color channel data that we have to reassemble again...
    else
    {
        m_nrOfuint16Pixels = m_nrOfPixels;
        // we convert the image to 16bit. check if to X1R5G5B5 or A1R5G5B5
        const bool useAlpha = image.matte() && params.device != "GBA";
        // reserve data twice the number of pixels
        m_data.resize(m_nrOfPixels * 2);
        unsigned short * pixels = reinterpret_cast<unsigned short*>(m_data.data());
        const Magick::PixelPacket *pixelData = image.getConstPixels(0, 0, width, height);
        for (unsigned int i = 0; i < (m_nrOfPixels); ++i)
        {
            auto ic = pixelData[i];
            unsigned char r = MagickCore::ScaleQuantumToChar(ic.red);
            unsigned char g = MagickCore::ScaleQuantumToChar(ic.green);
            unsigned char b = MagickCore::ScaleQuantumToChar(ic.blue);
            unsigned char a = MagickCore::ScaleQuantumToChar(ic.opacity);
            auto c16 = Color16(r, g, b, a);
            pixels[i] = useAlpha ? c16.ToDSShort() : c16.ToGBAShort();
        }
    }
}

std::string ImageDirect::GetImageType() const
{
    return m_imageTypeString + "*";
}

bool ImageDirect::HasPalette() const
{
    return m_palette ? m_palette->Size() > 0 : false;
}

void ImageDirect::WriteData(std::ostream &file) const
{
    // Sole owner of palette
    if (m_palette)
    {
        m_palette->WriteData(file);
    }
    WriteShortArray(file, name, "", m_data, 16);
    WriteNewLine(file);
}

void ImageDirect::WriteCommonExport(std::ostream &file) const
{
    WriteDefine(file, name, "_SIZE", m_data.size());
    WriteDefine(file, name, "_LENGTH", m_nrOfuint16Pixels);
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
}

void ImageDirect::WriteExport(std::ostream &file) const
{
    // Sole owner of palette
    if (m_palette)
    {
        m_palette->WriteExport(file);
    }
    WriteExtern(file, m_imageTypeString, name, "", m_nrOfuint16Pixels);
    WriteCommonExport(file);
    WriteNewLine(file);
}
