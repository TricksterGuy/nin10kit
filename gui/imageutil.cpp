#include "imageutil.hpp"

#include "image16.hpp"
#include "logger.hpp"

extern ExportParams params;

void DefaultParams()
{
    params.transparent_color = -1;
    params.dither_level = 10 / 100.0f;
    params.offset = 0;
    params.palette = 256;
    params.border = 0;
}

ImageInfo::ImageInfo(const std::string& _filename) : filename(_filename)
{
    name = Format(filename);
    readImages(&images, filename);
    width = images[0].columns();
    height = images[0].rows();
    thumbnail = MagickToBitmap(images[0], THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
}

void ConvertToMode3(std::map<std::string, ImageInfo>& images, std::vector<Image16Bpp>& images16)
{
    DefaultParams();
    for (const auto& filename_imageinfo : images)
    {
        const auto& filename = filename_imageinfo.first;
        const auto& info = filename_imageinfo.second;
        for (auto image : info.GetImages())
        {
            Magick::Geometry geom(info.GetWidth(), info.GetHeight());
            geom.aspect(true);
            image.resize(geom);
            images16.emplace_back(Image16Bpp(Image32Bpp(image, info.GetName(), filename, image.scene(), info.IsAnimated())));
        }
    }
}

void ConvertToMode4(std::map<std::string, ImageInfo>& images, std::vector<Image8Bpp>& images8)
{
    DefaultParams();
    std::vector<Image16Bpp> images16;
    ConvertToMode3(images, images16);
    //Image8BppScene scene(images16, "");
    //for (unsigned int i = 0; i < scene.NumImages(); i++)
        images8.emplace_back(images16[0]);
}

wxBitmap MagickToBitmap(Magick::Image image, int width, int height)
{
    Magick::Geometry geom(width, height);
    geom.aspect(true);
    image.resize(geom);
    unsigned char* rgb = static_cast<unsigned char*>(malloc(sizeof(char) * 3 * width * height));
    image.write(0, 0, image.columns(), image.rows(), "RGB", Magick::CharPixel, rgb);
    return wxBitmap(wxImage(width, height, rgb));
}

void TransferToWx(const Image16Bpp& image, wxImage& wx)
{
    wx.Create(image.width, image.height);

    for (unsigned int i = 0; i < image.height; i++)
    {
        for (unsigned int j = 0; j < image.width; j++)
        {
            const Color16& c = image.At(j, i);
            wx.SetRGB(j, i, c.r << 3, c.g << 3, c.b << 3);
        }
    }
}

void TransferToWx(const Image8Bpp& image, wxImage& wx)
{
    wx.Create(image.width, image.height);

    for (unsigned int i = 0; i < image.height; i++)
    {
        for (unsigned int j = 0; j < image.width; j++)
        {
            unsigned char index = image.At(j, i);
            const Color16& c = image.palette->At(index);
            wx.SetRGB(j, i, c.r << 3, c.g << 3, c.b << 3);
        }
    }
}

void TransferToWx(const Palette& palette, wxImage& wx)
{
    wx.Create(16, 16);
    for (unsigned int i = 0; i < palette.Size(); i++)
    {
        const Color16& c = palette.At(i);
        InfoLog("%d %d %d\n", c.r, c.g, c.b);
        wx.SetRGB(i % 16, i / 16, c.r << 3, c.g << 3, c.b << 3);
    }
}

void TransferToNin10Kit(const wxImage& wx, Image8Bpp& image)
{

}
