#include "tile.hpp"

#include "logger.hpp"
#include "fileutils.hpp"
#include "image16.hpp"
#include "image8.hpp"
#include "mediancut.hpp"
#include "shared.hpp"

ImageTile::ImageTile(const Image16Bpp& image, int tilex, int tiley, int border) : id(0), pixels(TILE_SIZE)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            pixels[i * 8 + j] = image.pixels[(tiley * (8+border) + i) * image.width + tilex * (8+border) + j];
    }
}

bool ImageTile::operator==(const ImageTile& other) const
{
    return IsEqual(other);
}

bool ImageTile::operator<(const ImageTile& other) const
{
    return pixels < other.pixels;
}

bool ImageTile::IsEqual(const ImageTile& other) const
{
    return pixels == other.pixels;
}

bool ImageTile::IsSameAs(const ImageTile& other) const
{
    bool same, sameh, samev, samevh;
    same = sameh = samev = samevh = true;
    for (int i = 0; i < TILE_SIZE; i++)
    {
        int x = i % 8;
        int y = i / 8;
        same = same && pixels[i] == other.pixels[i];
        samev = samev && pixels[i] == other.pixels[(7 - y) * 8 + x];
        sameh = sameh && pixels[i] == other.pixels[y * 8 + (7 - x)];
        samevh = samevh && pixels[i] == other.pixels[(7 - y) * 8 + (7 - x)];
    }
    return same || samev || sameh || samevh;
}

Tile::Tile(const Image16Bpp& image, int tilex, int tiley, int border, int _bpp) : id(0), pixels(TILE_SIZE), bpp(_bpp), palette_bank(-1),
    sourceTile(new ImageTile(image, tilex, tiley, border))
{
    const std::vector<Color16>& imgdata = sourceTile->pixels;
    unsigned int num_colors = 1 << bpp;

    GetPalette(imgdata, num_colors, params.transparent_color, 0, palette);
    ReduceImage(imgdata, palette, params.transparent_color, 0, pixels);
}

Tile::Tile(const Image8Bpp& image, int tilex, int tiley, int border, int _bpp) : id(0), pixels(TILE_SIZE), bpp(_bpp), palette_bank(-1), palette(*image.palette)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            pixels[i * 8 + j] = image.pixels[(tiley * (8+border) + i) * image.width + tilex * (8+border) + j];
    }
}

Tile::Tile(const Image16Bpp& image, std::shared_ptr<Palette>& global_palette, int tilex, int tiley, int border) : id(0), pixels(TILE_SIZE), bpp(8), palette_bank(-1),
    palette(*global_palette), sourceTile(new ImageTile(image, tilex, tiley, border))
{
    const std::vector<Color16>& imgdata = sourceTile->pixels;
    unsigned int num_colors = 1 << bpp;

    GetPalette(imgdata, num_colors, params.transparent_color, 0, palette);
    ReduceImage(imgdata, palette, params.transparent_color, 0, pixels);
}

Tile::Tile(std::shared_ptr<ImageTile>& imageTile, int _bpp) : id(0), pixels(TILE_SIZE), bpp(_bpp), palette_bank(-1), sourceTile(imageTile)
{
    const std::vector<Color16>& imgdata = sourceTile->pixels;
    unsigned int num_colors = 1 << bpp;

    GetPalette(imgdata, num_colors, params.transparent_color, 0, palette);
    ReduceImage(imgdata, palette, params.transparent_color, 0, pixels);
}

bool Tile::operator<(const Tile& other) const
{
    return pixels < other.pixels;
}

bool Tile::operator==(const Tile& other) const
{
    return IsEqual(other);
}

bool Tile::IsEqual(const Tile& other) const
{
    return pixels == other.pixels;
}

bool Tile::IsSameAs(const Tile& other) const
{
    bool same, sameh, samev, samevh;
    same = sameh = samev = samevh = true;
    for (int i = 0; i < TILE_SIZE; i++)
    {
        int x = i % 8;
        int y = i / 8;
        same = same && pixels[i] == other.pixels[i];
        samev = samev && pixels[i] == other.pixels[(7 - y) * 8 + x];
        sameh = sameh && pixels[i] == other.pixels[y * 8 + (7 - x)];
        samevh = samevh && pixels[i] == other.pixels[(7 - y) * 8 + (7 - x)];
    }
    return same || samev || sameh || samevh;
}

void Tile::UsePalette(const PaletteBank& bank)
{
    if (bpp == 8)
    {
        FatalLog("Calling UsePalette on a tile whose bpp is 8. This shouldn't happen");
        return;
    }

    std::map<int, int> remapping;
    for (unsigned int i = 0; i < palette.Size(); i++)
    {
        const Color16& old = palette.At(i);
        remapping[i] = bank.Search(old);
    }

    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        if (remapping.find(pixels[i]) == remapping.end())
            FatalLog("Somehow tile contains invalid indicies. This shouldn't happen");

        pixels[i] = remapping[pixels[i]];
    }

    palette_bank = bank.id;
    palette.Set(bank.GetColors());
}

std::ostream& operator<<(std::ostream& file, const Tile& tile)
{
    char buffer[7];
    const std::vector<unsigned char>& pixels = tile.pixels;
    if (tile.bpp == 8)
    {
        for (unsigned int i = 0; i < TILE_SIZE_SHORTS_8BPP; i++)
        {
            snprintf(buffer, 7, "0x%02x%02x", pixels[2 * i + 1], pixels[2 * i]);
            WriteElement(file, buffer, TILE_SIZE_SHORTS_8BPP, i, 8);
        }
    }
    else
    {
        for (unsigned int i = 0; i < TILE_SIZE_SHORTS_4BPP; i++)
        {
            snprintf(buffer, 7, "0x%01x%01x%01x%01x", pixels[4 * i + 3], pixels[4 * i + 2], pixels[4 * i + 1], pixels[4 * i]);
            WriteElement(file, buffer, TILE_SIZE_SHORTS_4BPP, i, 8);
        }
    }
    return file;
}

bool TilesPaletteSizeComp(const Tile& i, const Tile& j)
{
    return i.palette.Size() > j.palette.Size();
}
