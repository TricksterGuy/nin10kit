#include "imageutil.hpp"

#include "image16.hpp"

ImageInfo::ImageInfo(const std::string& _filename) : filename(_filename)
{
    name = Format(filename);
    readImages(&images, filename);
    width = images[0].columns();
    height = images[0].rows();
    thumbnail = MagickToBitmap(images[0], THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
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
            //Color c = image.At(j, i);
            unsigned char r, g, b;
            //c.GetRGB24(r, g, b);
            wx.SetRGB(j, i, r, g, b);
        }
    }
}

void TransferToNin10Kit(const wxImage& wx, Image16Bpp& image)
{

}
