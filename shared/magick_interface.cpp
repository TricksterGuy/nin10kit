#include "magick_interface.hpp"
#include "logger.hpp"
#include <algorithm>

#include "export_params.hpp"

#ifdef MAGICK7_SUPPORT
// Moan... Apparently these guys can be negative or outside quantum range
unsigned int clamp_quantum(Magick::Quantum q, size_t depth)
{
    int ret = (int) q;
    return std::min(std::max(0, ret), (1 << depth) - 1);
}

#endif // MAGICK7_SUPPORT

// TODO clean this up this is a really confusing interface
class MagickImageDataWrapper
{
public:
    MagickImageDataWrapper(const Magick::Image& image) : constPixels(image.getConstPixels(0, 0, image.columns(), image.rows())), pixels(nullptr), channels(0)
    {
#ifdef MAGICK6_SUPPORT
        alpha_mask = image.matte() ? 0 : 0xFF;
#endif
        MagickCore::GetMagickQuantumDepth(&depth);
        if (depth != 8 && depth != 16)
            FatalLog("Image quantum not supported");
#ifdef MAGICK7_SUPPORT
        channels = image.channels();
#endif
    }
    MagickImageDataWrapper(Magick::Image& img) : constPixels(nullptr), pixels(img.getPixels(0, 0, img.columns(), img.rows())), channels(0)
    {
#ifdef MAGICK6_SUPPORT
        alpha_mask = img.matte() ? 0 : 0xFF;
#endif
        MagickCore::GetMagickQuantumDepth(&depth);
        if (depth != 8 && depth != 16)
            FatalLog("Image quantum not supported");
#ifdef MAGICK7_SUPPORT
        channels = img.channels();
#endif
    }
    void getPixel(int index, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const
    {
#ifdef MAGICK6_SUPPORT
        const Magick::PixelPacket& p = constPixels[index];
        r = p.red >> (depth - 8) & 0xFF;
        g = p.green >> (depth - 8) & 0xFF;
        b = p.blue >> (depth - 8) & 0xFF;
        a = (0xff - (p.opacity >> (depth - 8) & 0xFF)) | alpha_mask;
#endif
#ifdef MAGICK7_SUPPORT
        r = g = b = a = 0;
        const Magick::Quantum* loc = constPixels + index * channels;
        r = clamp_quantum(loc[0], depth) >> (depth - 8) & 0xFF;
        g = clamp_quantum(loc[1], depth) >> (depth - 8) & 0xFF;
        b = clamp_quantum(loc[2], depth) >> (depth - 8) & 0xFF;
        if (channels == 4)
            a = clamp_quantum(loc[3], depth) >> (depth - 8) & 0xFF;
        else
            a = 0xFF;
#endif
        if (a == 0)
        {
            r = 0;
            g = 0;
            b = 0;
        }
    }

    void setPixel(int index, const Color& color)
    {
#ifdef MAGICK6_SUPPORT
        Magick::PixelPacket& p = pixels[index];
        p.red = color.r << (depth - 8);
        p.green = color.g << (depth - 8);
        p.blue = color.b << (depth - 8);
#endif
#ifdef MAGICK7_SUPPORT
        Magick::Quantum* loc = pixels + index * channels;
        loc[0] = color.r << (depth - 8);
        loc[1] = color.g << (depth - 8);
        loc[2] = color.b << (depth - 8);
#endif
    }
private:
#ifdef MAGICK6_SUPPORT
    const Magick::PixelPacket* constPixels;
    Magick::PixelPacket* pixels;
    unsigned char alpha_mask;
#endif
#ifdef MAGICK7_SUPPORT
    const Magick::Quantum* constPixels;
    Magick::Quantum* pixels;
#endif
    size_t channels;
    size_t depth;
};

void CopyMagickPixels(const Magick::Image& image, std::vector<Color>& out)
{
    unsigned int num_pixels = image.rows() * image.columns();
    MagickImageDataWrapper imageData(image);
    for (unsigned int i = 0; i < num_pixels; i++)
    {
        unsigned char r, g, b, a;
        imageData.getPixel(i, r, g, b, a);
        if (a == 0 && params.transparent_given)
            out.emplace_back(params.transparent_color);
        else
            out.emplace_back(r, g, b, a);
    }
}

Magick::Image ToMagick(const Image32Bpp& image)
{
    Magick::Image ret(Magick::Geometry(image.width, image.height), Magick::Color(0, 0, 0));
    MagickImageDataWrapper imageData(ret);
    for (unsigned int i = 0; i < image.height; i++)
    {
        for (unsigned int j = 0; j < image.width; j++)
            imageData.setPixel(i * image.width + j, image.pixels[i * image.width + j]);
    }
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const Image16Bpp& image)
{
    Magick::Image ret(Magick::Geometry(image.width, image.height), Magick::Color(0, 0, 0));
    MagickImageDataWrapper imageData(ret);
    for (unsigned int i = 0; i < image.height; i++)
    {
        for (unsigned int j = 0; j < image.width; j++)
            imageData.setPixel(i * image.width + j, image.pixels[i * image.width + j].ToColor());
    }
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const Image8Bpp& image)
{
    Magick::Image ret(Magick::Geometry(image.width, image.height), Magick::Color(0, 0, 0));
    MagickImageDataWrapper imageData(ret);
    for (unsigned int i = 0; i < image.height; i++)
    {
        for (unsigned int j = 0; j < image.width; j++)
            imageData.setPixel(i * image.width + j, image.palette->At(image.pixels[i * image.width + j]).ToColor());
    }
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const Palette& palette)
{
    Magick::Image ret(Magick::Geometry(16, 16), Magick::Color(0, 0, 0));
    MagickImageDataWrapper imageData(ret);
    const std::vector<Color16>& colors = palette.GetColors();
    for (unsigned int i = 0; i < colors.size(); i++)
        imageData.setPixel(i, colors[i].ToColor());
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const PaletteBankManager& paletteBanks)
{
    Magick::Image ret(Magick::Geometry(16, 16), Magick::Color(0, 0, 0));
    MagickImageDataWrapper imageData(ret);
    for (unsigned int i = 0; i < paletteBanks.banks.size(); i++)
    {
        const std::vector<Color16>& colors = paletteBanks.banks[i].GetColors();
        for (unsigned int j = 0; j < colors.size(); i++)
            imageData.setPixel(i, colors[j].ToColor());
    }
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const Tileset& tileset)
{
    const int tilesX = 32;
    const int tilesY = 32;
    const int width = tilesX * 8;
    const int height = tilesY * 8;
    const std::vector<Tile>& tiles = tileset.tilesExport;
    Magick::Image ret(Magick::Geometry(width, height), Magick::Color(0, 0, 0));
    MagickImageDataWrapper imageData(ret);

    if (tileset.bpp == 4)
    {
        for (unsigned int i = 0; i < tiles.size(); i++)
        {
            const Tile& tile = tiles[i];
            const PaletteBank& palette = tileset.paletteBanks[tile.palette_bank];
            int sx = i % tilesX * 8;
            int sy = i / tilesX * 8;
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                imageData.setPixel(sx + j % 8 + (sy + j / 8) * width, palette.At(pix).ToColor());
            }
        }
    }
    else
    {
        const Palette& palette = *tileset.palette;
        for (unsigned int i = 0; i < tiles.size(); i++)
        {
            const Tile& tile = tiles[i];
            int sx = i % tilesX * 8;
            int sy = i / tilesX * 8;
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                imageData.setPixel(sx + j % 8 + (sy + j / 8) * width, palette.At(pix).ToColor());
            }
        }
    }
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const Sprite& sprite, const std::vector<PaletteBank>& banks)
{
    const int tilesX = sprite.width;
    const int tilesY = sprite.height;
    const int width = tilesX * 8;
    const int height = tilesY * 8;
    Magick::Image ret(Magick::Geometry(width, height), Magick::Color(0, 0, 0));
    MagickImageDataWrapper imageData(ret);

    if (sprite.bpp == 4)
    {
        for (unsigned int i = 0; i < sprite.data.size(); i++)
        {
            const Tile& tile = sprite.data[i];
            const PaletteBank& palette = banks[sprite.palette_bank];
            int sx = i % tilesX * 8;
            int sy = i / tilesX * 8;
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                imageData.setPixel(sx + j % 8 + (sy + j / 8) * width, palette.At(pix).ToColor());
            }
        }
    }
    else
    {
        const Palette& palette = *sprite.palette;
        for (unsigned int i = 0; i < sprite.data.size(); i++)
        {
            const Tile& tile = sprite.data[i];
            int sx = i % tilesX * 8;
            int sy = i / tilesX * 8;
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                imageData.setPixel(sx + j % 8 + (sy + j / 8) * width, palette.At(pix).ToColor());
            }
        }
    }
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const Map& map)
{
    const int tilesX = map.width;
    const int tilesY = map.height;
    const int width = tilesX * 8;
    const int height = tilesY * 8;
    Magick::Image ret(Magick::Geometry(width, height), Magick::Color(0, 0, 0));
    MagickImageDataWrapper imageData(ret);

    const Tileset& tileset = *map.tileset;
    if (tileset.bpp == 4)
    {
        for (unsigned int i = 0; i < map.data.size(); i++)
        {
            int sx = i % map.width * 8;
            int sy = i / map.width * 8;
            int tile_id = map.data[i] & 0x3FF;
            int pal_id = (map.data[i] >> 12) & 0xF;
            const Tile& tile = tileset.tilesExport[tile_id];
            const PaletteBank& palette = tileset.paletteBanks[pal_id];
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                if (!pix) continue;
                imageData.setPixel(sx + j % 8 + (sy + j / 8) * width, palette.At(pix).ToColor());
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < map.data.size(); i++)
        {
            int sx = i % map.width * 8;
            int sy = i / map.width * 8;
            int tile_id = map.data[i] & 0x3FF;
            const Tile& tile = tileset.tilesExport[tile_id];
            const Palette& palette = *tileset.palette;
            for (unsigned int j = 0; j < TILE_SIZE; j++)
            {
                unsigned char pix = tile.pixels[j];
                if (!pix) continue;
                imageData.setPixel(sx + j % 8 + (sy + j / 8) * width, palette.At(pix).ToColor());
            }
        }
    }
    ret.syncPixels();
    return ret;
}
