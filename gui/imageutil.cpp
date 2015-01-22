#include "imageutil.hpp"

#include "image16.hpp"
#include "logger.hpp"

extern ExportParams params;

void DefaultParams()
{
    params.transparent_color = Color(0, 0, 0);
    params.transparent_given = false;
    params.dither_level = 10 / 100.0f;
    params.offset = 0;
    params.palette = 256;
    params.border = 0;
    params.force = true;
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
    Image8BppScene scene(images16, "");
    for (unsigned int i = 0; i < scene.NumImages(); i++)
        images8.emplace_back(scene.GetImage(i));
}

void ConvertToMode0(std::map<std::string, ImageInfo>& images, std::vector<Map>& maps, int bpp)
{
    DefaultParams();
    params.bpp = bpp;
    std::vector<Image16Bpp> images16;
    ConvertToMode3(images, images16);
    MapScene scene(images16, "", bpp);
    for (unsigned int i = 0; i < scene.NumImages(); i++)
        maps.emplace_back(scene.GetMap(i));
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
        wx.SetRGB(i % 16, i / 16, c.r << 3, c.g << 3, c.b << 3);
    }
}

void TransferToWx(const PaletteBankManager& pbmanager, wxImage& wx)
{
    wx.Create(16, 16);
    for (unsigned int bank_id = 0; bank_id < pbmanager.Size(); bank_id++)
    {
        const PaletteBank& bank = pbmanager[bank_id];
        int bx = (bank_id % 4) * 4;
        int by = (bank_id / 4) * 4;
        for (unsigned int i = 0; i < bank.Size(); i++)
        {
            const Color16& c = bank.At(i);
            wx.SetRGB(i % 4 + bx, i / 4 + by, c.r << 3, c.g << 3, c.b << 3);
        }
    }
}

void TransferToWx(const Tileset& tileset, wxImage& wx)
{
    // 16 x 64
    const int tilesX = 32;
    const int tilesY = 32;
    wx.Create(tilesX * 8, tilesY * 8);
    int bpp = tileset.bpp;
    if (bpp == 4)
    {
        for (unsigned int i = 0; i < tileset.tilesExport.size(); i++)
        {
            const Tile& tile = tileset.tilesExport[i];
            const PaletteBank& palette = tileset.paletteBanks[tile.palette_bank];
            int tx = i % tilesX;
            int ty = i / tilesX;
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                const auto& c = palette.At(pix);
                wx.SetRGB(tx * 8 + j % 8, ty * 8 + j / 8, c.r << 3, c.g << 3, c.b << 3);
            }
        }
    }
    else
    {
        const Palette& palette = *tileset.palette;
        for (unsigned int i = 0; i < tileset.tilesExport.size(); i++)
        {
            const Tile& tile = tileset.tilesExport[i];
            int tx = i % tilesX;
            int ty = i / tilesX;
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                const auto& c = palette.At(pix);
                wx.SetRGB(tx * 8 + j % 8, ty * 8 + j / 8, c.r << 3, c.g << 3, c.b << 3);
            }
        }
    }
}

void TransferToWx(const Map& map, wxImage& wx)
{
    wx.Create(map.width * 8, map.height * 8);
    if (map.tileset->bpp == 4)
    {
        for (unsigned int i = 0; i < map.data.size(); i++)
        {
            int x = i % map.width;
            int y = i / map.width;
            int tile_id = map.data[i] & 0x3FF;
            int pal_id = (map.data[i] >> 12) & 0xF;
            const Tile& tile = map.tileset->tilesExport[tile_id];
            const PaletteBank& palette = map.tileset->paletteBanks[pal_id];
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                if (!pix) continue;
                const auto& c = palette.At(pix);
                wx.SetRGB(x * 8 + j % 8, y * 8 + j / 8, c.r << 3, c.g << 3, c.b << 3);
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < map.data.size(); i++)
        {
            int x = i % map.width;
            int y = i / map.width;
            int tile_id = map.data[i] & 0x3FF;
            const Tile& tile = map.tileset->tilesExport[tile_id];
            const Palette& palette = *map.tileset->palette;
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                if (!pix) continue;
                const auto& c = palette.At(pix);
                wx.SetRGB(x * 8 + j % 8, y * 8 + j / 8, c.r << 3, c.g << 3, c.b << 3);
            }
        }
    }
}

void TransferToNin10Kit(const wxImage& wx, Image8Bpp& image)
{

}
