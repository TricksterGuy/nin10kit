#include "magick_interface.hpp"
#include "logger.hpp"

Magick::Color ToMagickColor(const Color& color)
{
    size_t depth;
    MagickCore::GetMagickQuantumDepth(&depth);
    const unsigned int shift = 16 - depth;
    return Magick::Color(color.r << shift, color.g << shift, color.b << shift);
}

void CopyMagickPixels(const Magick::Image& image, std::vector<Color>& out)
{
    unsigned int num_pixels = image.rows() * image.columns();
    const Magick::PixelPacket* imageData = image.getConstPixels(0, 0, image.columns(), image.rows());

    size_t depth;
    MagickCore::GetMagickQuantumDepth(&depth);
    unsigned char alpha_mask = image.matte() ? 0 : 0xFF;
    for (unsigned int i = 0; i < num_pixels; i++)
    {
        const Magick::PixelPacket& packet = imageData[i];
        unsigned char r, g, b, a;
        if (depth == 8)
        {
            r = packet.red;
            g = packet.green;
            b = packet.blue;
            a = 0xff - packet.opacity;
        }
        else if (depth == 16)
        {
            r = (packet.red >> 8) & 0xFF;
            g = (packet.green >> 8) & 0xFF;
            b = (packet.blue >> 8) & 0xFF;
            a = 0xff - ((packet.opacity >> 8) & 0xFF);
        }
        else
        {
            // To get rid of warning
            b = g = r = a = 0;
            FatalLog("Image quantum not supported");
        }

        out.emplace_back(r, g, b, a | alpha_mask);
    }
}

Magick::Image ToMagick(const Image32Bpp& image)
{
    Magick::Image ret(Magick::Geometry(image.width, image.height), Magick::Color(0, 0, 0));
    Magick::PixelPacket* packet = ret.getPixels(0, 0, image.width, image.height);
    for (unsigned int i = 0; i < image.height; i++)
    {
        for (unsigned int j = 0; j < image.width; j++)
        {
            packet[i * image.width + j] = ToMagickColor(image.pixels[i * image.width + j]);
        }
    }
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const Image16Bpp& image)
{
    Magick::Image ret(Magick::Geometry(image.width, image.height), Magick::Color(0, 0, 0));
    Magick::PixelPacket* packet = ret.getPixels(0, 0, image.width, image.height);
    for (unsigned int i = 0; i < image.height; i++)
    {
        for (unsigned int j = 0; j < image.width; j++)
        {
            packet[i * image.width + j] = ToMagickColor(image.pixels[i * image.width + j].ToColor());
        }
    }
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const Image8Bpp& image)
{
    Magick::Image ret(Magick::Geometry(image.width, image.height), Magick::Color(0, 0, 0));
    Magick::PixelPacket* packet = ret.getPixels(0, 0, image.width, image.height);
    for (unsigned int i = 0; i < image.height; i++)
    {
        for (unsigned int j = 0; j < image.width; j++)
        {
            packet[i * image.width + j] = ToMagickColor(image.palette->At(image.pixels[i * image.width + j]).ToColor());
        }
    }
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const Palette& palette)
{
    Magick::Image ret(Magick::Geometry(16, 16), Magick::Color(0, 0, 0));
    Magick::PixelPacket* packet = ret.getPixels(0, 0, 16, 16);
    const std::vector<Color16>& colors = palette.GetColors();
    for (unsigned int i = 0; i < colors.size(); i++)
        packet[i] = ToMagickColor(colors[i].ToColor());
    ret.syncPixels();
    return ret;
}

Magick::Image ToMagick(const PaletteBankManager& paletteBanks)
{
    Magick::Image ret(Magick::Geometry(16, 16), Magick::Color(0, 0, 0));
    Magick::PixelPacket* packet = ret.getPixels(0, 0, 16, 16);
    for (unsigned int i = 0; i < paletteBanks.banks.size(); i++)
    {
        const std::vector<Color16>& colors = paletteBanks.banks[i].GetColors();
        for (unsigned int j = 0; j < colors.size(); i++)
            packet[i] = ToMagickColor(colors[i].ToColor());

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
    Magick::PixelPacket* packet = ret.getPixels(0, 0, width, height);

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
                packet[sx + j % 8 + (sy + j / 8) * width] = ToMagickColor(palette.At(pix).ToColor());
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
                packet[sx + j % 8 + (sy + j / 8) * width] = ToMagickColor(palette.At(pix).ToColor());
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
    Magick::PixelPacket* packet = ret.getPixels(0, 0, width, height);

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
                packet[sx + j % 8 + (sy + j / 8) * width] = ToMagickColor(palette.At(pix).ToColor());
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
                packet[sx + j % 8 + (sy + j / 8) * width] = ToMagickColor(palette.At(pix).ToColor());
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
    Magick::PixelPacket* packet = ret.getPixels(0, 0, width, height);

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
                packet[sx + j % 8 + (sy + j / 8) * width] = ToMagickColor(palette.At(pix).ToColor());
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
                packet[sx + j % 8 + (sy + j / 8) * width] = ToMagickColor(palette.At(pix).ToColor());
            }
        }
    }
    ret.syncPixels();
    return ret;
}
