#include "reductionhelper.hpp"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <sstream>

#include "cpercep.hpp"
#include "dither.hpp"
#include "fileutils.hpp"
#include "mediancut.hpp"
#include "shared.hpp"

const int sprite_shapes[16] =
{
//h = 1,  2, 4,  8
      0,  2, 2, -1, // width = 1
      1,  0, 2, -1, // width = 2
      1,  1, 0,  2, // width = 4
     -1, -1, 1,  0  // width = 8
};

const int sprite_sizes[16] =
{
//h = 1,  2, 4,  8
      0,  0, 1, -1, // width = 1
      0,  1, 2, -1, // width = 2
      1,  2, 2,  3, // width = 4
     -1, -1, 3,  3  // width = 8
};

Image32Bpp::Image32Bpp(const Magick::Image& image, const std::string& name, const std::string& filename, unsigned int frame, bool animated) :
    Image(image.columns(), image.rows(), name, filename, frame, animated), pixels(width * height), has_alpha(false)
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

Image16Bpp::Image16Bpp(const Image32Bpp& image) : Image(image), pixels(width * height)
{
    unsigned int num_pixels = width * height;
    const std::vector<Color>& pixels32 = image.pixels;

    for (unsigned int i = 0; i < num_pixels; i++)
    {
        pixels[i].SetBGR15(pixels32[i]);
    }
}

void Image16Bpp::GetColors(std::vector<Color>& colors) const
{
    for (const auto& color : pixels)
    {
        if (params.transparent_color != color)
            colors.push_back(color);
    }
}

void Image16Bpp::WriteData(std::ostream& file) const
{
    char buffer[7];
    WriteBeginArray(file, "const unsigned short", export_name, "", pixels.size());
    for (unsigned int i = 0; i < pixels.size(); i++)
    {
        snprintf(buffer, 7, "0x%04x", pixels[i].GetBGR15());
        WriteElement(file, buffer, pixels.size(), i, 16);
    }
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

void ColorArray::Set(const std::vector<Color>& _colors)
{
    colors = _colors;
    colorSet.clear();
    colorSet.insert(colors.begin(), colors.end());
}

int ColorArray::Search(const Color& a) const
{
    double bestd = DBL_MAX;
    int index = -1;

    if (colorIndexCache.find(a) != colorIndexCache.end())
        return colorIndexCache[a];

    for (unsigned int i = 0; i < colors.size(); i++)
    {
        const Color& b = colors[i];
        double dist = a.Distance(b);
        if (dist < bestd)
        {
            index = i;
            bestd = dist;
            if (bestd == 0) break;
        }
    }

    colorIndexCache[a] = index;

    //if (bestd != 0)
    //    VerboseLog("Color remap: Color 0x%08x given to palette not an exact match. palette entry: %d - 0x%08x.  dist: %f.", a.GetARGB(), index, colors[index].GetARGB(), bestd);

    return index;
}

bool ColorArray::Contains(const ColorArray& palette) const
{
    for (const auto& color : palette.colors)
    {
        if (colorSet.find(color) == colorSet.end())
            return false;
    }
    return true;
}

void ColorArray::Add(const Color& c)
{
    if (colorSet.find(c) == colorSet.end())
    {
        colorSet.insert(c);
        colors.push_back(c);
    }
}

Palette::Palette(const std::vector<Color>& colors, const std::string& name) : ColorArray(colors), Exportable(name)
{
    if (colors.size() + params.offset > 256)
        FatalLog("Too many colors in palette. Found %d colors, offset is %d.", colors.size() + params.offset, params.offset);
}

void Palette::WriteData(std::ostream& file) const
{
    char buffer[7];
    WriteBeginArray(file, "const unsigned short", name, "_palette", colors.size());
    for (unsigned int i = 0; i < colors.size(); i++)
    {
        snprintf(buffer, 7, "0x%04x", colors[i].GetBGR15());
        WriteElement(file, buffer, colors.size(), i, 16);
    }
    WriteEndArray(file);
    WriteNewLine(file);
}

void Palette::WriteExport(std::ostream& file) const
{
    WriteExtern(file, "const unsigned short", name, "_palette", colors.size());
    WriteDefine(file, name, "_PALETTE_SIZE", colors.size());
    WriteNewLine(file);
}

Image8Bpp::Image8Bpp(const Image16Bpp& image) : Image(image), pixels(width * height), palette(new Palette(export_name)), export_shared_info(true)
{
    // If the image width is odd error out
    if (width & 1)
        FatalLog("Image: %s width is not a multiple of 2. Please fix", name.c_str());

    std::vector<Color> pixels16;
    image.GetColors(pixels16);

    GetPalette(pixels16, params.palette, params.transparent_color, params.offset, *palette);
    DitherAndReduceImage(image, params.transparent_color, params.dither, params.dither_level, params.offset, *this);
}

Image8Bpp::Image8Bpp(const Image16Bpp& image, std::shared_ptr<Palette>& global_palette) : Image(image), pixels(width * height), palette(global_palette), export_shared_info(false)
{
    if (width & 1)
        FatalLog("Image: %s width is not a multiple of 2. Please fix", name.c_str());

    DitherAndReduceImage(image, params.transparent_color, params.dither, params.dither_level, params.offset, *this);
}

void Image8Bpp::WriteData(std::ostream& file) const
{
    // Sole owner of palette
    if (export_shared_info)
        palette->WriteData(file);
    WriteShortArray(file, export_name, "", pixels, 16);
    WriteNewLine(file);
}

void Image8Bpp::WriteCommonExport(std::ostream& file) const
{
    WriteDefine(file, name, "_SIZE", pixels.size() / 2);
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
}

void Image8Bpp::WriteExport(std::ostream& file) const
{
    // Sole owner of palette
    if (export_shared_info)
        palette->WriteExport(file);
    WriteExtern(file, "const unsigned short", export_name, "", pixels.size() / 2);
    if (!animated)
    {
        WriteDefine(file, export_name, "_SIZE", pixels.size() / 2);
        WriteDefine(file, export_name, "_WIDTH", width);
        WriteDefine(file, export_name, "_HEIGHT", height);
    }
    WriteNewLine(file);
}

Image8BppScene::Image8BppScene(const std::vector<Image16Bpp>& images16, const std::string& name) : Scene(name), palette(new Palette(name))
{
    std::vector<Color> pixels;
    for (unsigned int i = 0; i < images16.size(); i++)
        images16[i].GetColors(pixels);

    GetPalette(pixels, params.palette, params.transparent_color, params.offset, *palette);

    images.reserve(images16.size());
    for (unsigned int i = 0; i < images16.size(); i++)
        images.emplace_back(new Image8Bpp(images16[i], palette));
}

const Image8Bpp& Image8BppScene::GetImage(int index) const
{
    const Image* image = images[index].get();
    const Image8Bpp* image8 = dynamic_cast<const Image8Bpp*>(image);
    if (!image8) FatalLog("Could not cast Image to Image8Bpp. This shouldn't happen");
    return *image8;
}

void Image8BppScene::WriteData(std::ostream& file) const
{
    palette->WriteData(file);
    Scene::WriteData(file);
}

void Image8BppScene::WriteExport(std::ostream& file) const
{
    palette->WriteExport(file);
    Scene::WriteExport(file);
}

int PaletteBank::CanMerge(const ColorArray& palette) const
{
    std::set<Color> tmpset(colorSet);
    for (const auto& color : palette.GetColors())
        tmpset.insert(color);

    return 16 - tmpset.size();
}

void PaletteBank::Merge(const ColorArray& palette)
{
    for (const auto& color : palette.GetColors())
        Add(color);
}

std::ostream& operator<<(std::ostream& file, const PaletteBank& bank)
{
    std::vector<Color> colors = bank.GetColors();
    colors.resize(16);

    char buffer[7];
    for (unsigned int i = 0; i < colors.size(); i++)
    {
        snprintf(buffer, 7, "0x%04x", colors[i].GetBGR15());
        WriteElement(file, buffer, colors.size(), i, 8);
    }

    return file;
}

PaletteBankManager::PaletteBankManager(const std::string& name) : Exportable(name), banks(16)
{
    for (unsigned int i = 0; i < banks.size(); i++)
        banks[i].id = i;
}

PaletteBankManager::PaletteBankManager(const std::string& _name, const std::vector<PaletteBank>& paletteBanks) : Exportable(_name), banks(paletteBanks)
{
}

void PaletteBankManager::WriteData(std::ostream& file) const
{
    file << "const unsigned short " << name << "_palette[256] =\n{\n\t";
    for (unsigned int i = 0; i < 16; i++)
    {
        file << banks[i];
        if (i != 16 - 1)
            file << ",\n\t";
    }
    file << "\n};\n";
    WriteNewLine(file);
}

void PaletteBankManager::WriteExport(std::ostream& file) const
{
    WriteExtern(file, "const unsigned short", name, "_palette", 256);
    WriteDefine(file, name, "_PALETTE_SIZE", 256);
    WriteNewLine(file);
}

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
    const std::vector<Color>& imgdata = sourceTile->pixels;
    unsigned int num_colors = 1 << bpp;

    GetPalette(imgdata, num_colors, params.transparent_color, 0, palette);
    QuantizeImage(imgdata, num_colors, params.transparent_color, 0, palette, pixels);
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
    const std::vector<Color>& imgdata = sourceTile->pixels;
    unsigned int num_colors = 1 << bpp;

    QuantizeImage(imgdata, num_colors, params.transparent_color, 0, palette, pixels);
}

Tile::Tile(std::shared_ptr<ImageTile>& imageTile, int _bpp) : id(0), pixels(TILE_SIZE), bpp(_bpp), palette_bank(-1), sourceTile(imageTile)
{
    const std::vector<Color>& imgdata = sourceTile->pixels;
    unsigned int num_colors = 1 << bpp;

    GetPalette(imgdata, num_colors, params.transparent_color, 0, palette);
    QuantizeImage(imgdata, num_colors, params.transparent_color, 0, palette, pixels);
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
        const Color& old = palette.At(i);
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

Tileset::Tileset(const std::vector<Image16Bpp>& images, const std::string& name, int _bpp) : Exportable(name), bpp(_bpp), paletteBanks(name)
{
    switch(bpp)
    {
        case 4:
            Init4bpp(images);
            break;
        case 8:
            Init8bpp(images);
            break;
        case 16:
            Init16bpp(images);
            break;
    }
}

Tileset::Tileset(const Image16Bpp& image, int _bpp) : Exportable(image), bpp(_bpp), paletteBanks(name)
{
    std::vector<Image16Bpp> images;
    images.push_back(image);
    switch(bpp)
    {
        case 4:
            Init4bpp(images);
            break;
        case 8:
            Init8bpp(images);
            break;
        case 16:
            Init16bpp(images);
            break;
    }
}

int Tileset::Search(const Tile& tile) const
{
    const std::set<Tile>::const_iterator foundTile = tiles.find(tile);
    if (foundTile != tiles.end())
        return foundTile->id;

    return -1;
}

int Tileset::Search(const ImageTile& tile) const
{
    const std::set<ImageTile>::const_iterator foundTile = itiles.find(tile);
    if (foundTile != itiles.end())
        return foundTile->id;

    return -1;
}

bool Tileset::Match(const ImageTile& tile, int& tile_id, int& pal_id) const
{
    const std::map<ImageTile, Tile>::const_iterator foundTile = matcher.find(tile);
    if (foundTile != matcher.end())
    {
        const Tile& tile = foundTile->second;
        tile_id = tile.id;
        pal_id = tile.palette_bank;
        return true;
    }

    return false;
}

void Tileset::WriteData(std::ostream& file) const
{
    if (bpp == 8)
        palette->WriteData(file);
    else
        paletteBanks.WriteData(file);

    WriteBeginArray(file, "const unsigned short", name, "_tiles", Size());
    std::vector<Tile>::const_iterator tile_ptr = tilesExport.begin();
    for (unsigned int i = 0; i < tilesExport.size(); i++)
    {
        file << *tile_ptr;
        if (i != tilesExport.size() - 1)
            file << ",\n\t";
        ++tile_ptr;
    }
    WriteEndArray(file);
    WriteNewLine(file);
}

void Tileset::WriteExport(std::ostream& file) const
{
    if (bpp == 8)
        palette->WriteExport(file);
    else
        paletteBanks.WriteExport(file);

    WriteDefine(file, name, "_PALETTE_TYPE", (bpp == 8), 7);
    WriteNewLine(file);

    WriteExtern(file, "const unsigned short", name, "_tiles", Size());
    WriteDefine(file, name, "_TILES", tiles.size());
    WriteDefine(file, name, "_TILES_SIZE", Size());
    WriteNewLine(file);
}

void Tileset::Init4bpp(const std::vector<Image16Bpp>& images)
{
    // Tile image into 16 bit tiles
    Tileset tileset16bpp(images, name, 16);
    std::set<ImageTile> imageTiles = tileset16bpp.itiles;

    const Tile& nullTile = Tile::GetNullTile4();
    const ImageTile& nullImageTile = ImageTile::GetNullTile();
    tiles.insert(nullTile);
    tilesExport.push_back(nullTile);
    matcher.insert(std::pair<ImageTile, Tile>(nullImageTile, nullTile));

    // Reduce each tile to 4bpp
    std::vector<Tile> gbaTiles;
    for (std::set<ImageTile>::const_iterator i = imageTiles.begin(); i != imageTiles.end(); ++i)
    {
        std::shared_ptr<ImageTile> src(new ImageTile(*i));
        Tile tile(src, 4);
        gbaTiles.push_back(tile);
    }

    // Ensure image contains < 256 colors
    std::set<Color> bigPalette;
    for (const auto& tile : gbaTiles)
    {
        const std::vector<Color>& tile_palette = tile.palette.GetColors();
        bigPalette.insert(tile_palette.begin(), tile_palette.end());
    }

    if (bigPalette.size() > 256 && !params.force)
        FatalLog("Image after reducing tiles to 4bpp still contains more than 256 distinct colors.  Found %d colors. Please fix.", bigPalette.size());

    // Greedy approach deal with tiles with largest palettes first.
    std::sort(gbaTiles.begin(), gbaTiles.end(), TilesPaletteSizeComp);

    // But deal with nulltile
    for (unsigned int i = 0; i < paletteBanks.Size(); i++)
        paletteBanks[i].Add(Color());

    // Construct palette banks, assign bank id to tile, remap tile to palette bank given, assign tile ids
    for (auto& tile : gbaTiles)
    {
        int pbank = -1;
        // Fully contains checks
        for (unsigned int i = 0; i < paletteBanks.Size(); i++)
        {
            PaletteBank& bank = paletteBanks[i];
            if (bank.Contains(tile.palette))
                pbank = i;
        }

        // Ok then find least affected bank
        if (pbank == -1)
        {
            int max_colors_left = -1;
            for (unsigned int i = 0; i < paletteBanks.Size(); i++)
            {
                PaletteBank& bank = paletteBanks[i];
                int colors_left = bank.CanMerge(tile.palette);
                if (colors_left != 0 && max_colors_left < colors_left)
                {
                    max_colors_left = colors_left;
                    pbank = i;
                }
            }
        }

        // Cry and die for now. Unless you tell me to keep going.
        if (pbank == -1 && !params.force)
            FatalLog("More than 16 distinct palettes found, please use 8bpp mode.");

        // Merge step and assign palette bank
        paletteBanks[pbank].Merge(tile.palette);
        tile.palette_bank = pbank;
        tile.UsePalette(paletteBanks[pbank]);

        // Assign tile id
        std::set<Tile>::const_iterator it = tiles.find(tile);
        if (it == tiles.end())
        {
            tile.id = tiles.size();
            tiles.insert(tile);
            tilesExport.push_back(tile);
        }
        else
        {
            tile.id = it->id;
        }

        // Form mapping from ImageTile to Tile
        matcher.insert(std::pair<ImageTile, Tile>(*tile.sourceTile, tile));
    }

    int tile_size = TILE_SIZE_BYTES_4BPP;
    int memory_b = tiles.size() * tile_size;
    if (tiles.size() >= 1024)
        FatalLog("Too many tiles found %d tiles. Please make the image simpler.", tiles.size());

    // Delicious infos
    int cbbs = tiles.size() * tile_size / SIZE_CBB_BYTES;
    int sbbs = (int) ceil(tiles.size() * tile_size % SIZE_CBB_BYTES / ((double)SIZE_SBB_BYTES));
    InfoLog("Tiles found %zu.", tiles.size());
    InfoLog("Tiles uses %d charblocks and %d screenblocks.", cbbs, sbbs);
    InfoLog("Total utilization %.2f/4 charblocks or %d/32 screenblocks, %d/65536 bytes.",
           memory_b / ((double)SIZE_CBB_BYTES), (int) ceil(memory_b / ((double)SIZE_SBB_BYTES)), memory_b);
}

void Tileset::Init8bpp(const std::vector<Image16Bpp>& images16)
{
    int tile_width = 8 + params.border;

    // Reduce all and get the global palette and reduced images.
    Image8BppScene scene(images16, name);
    palette = scene.palette;

    const Tile& nullTile = Tile::GetNullTile8();
    const ImageTile& nullImageTile = ImageTile::GetNullTile();
    tiles.insert(nullTile);
    tilesExport.push_back(nullTile);
    matcher.insert(std::pair<ImageTile, Tile>(nullImageTile, nullTile));

    for (unsigned int k = 0; k < images16.size(); k++)
    {
        bool disjoint_error = false;
        const Image8Bpp& image = scene.GetImage(k);
        const Image16Bpp& image16 = images16[k];

        offsets.push_back(tiles.size());
        unsigned int tilesX = image.width / tile_width;
        unsigned int tilesY = image.height / tile_width;
        unsigned int totalTiles = tilesX * tilesY;

        // Perform reduce.
        for (unsigned int i = 0; i < totalTiles; i++)
        {
            int tilex = i % tilesX;
            int tiley = i / tilesX;
            Tile tile(image, tilex, tiley, params.border);
            std::set<Tile>::iterator foundTile = tiles.find(tile);
            if (foundTile == tiles.end())
            {
                tile.id = tiles.size();
                tiles.insert(tile);
                tilesExport.push_back(tile);
                // Add matcher data
                ImageTile imageTile(image16, tilex, tiley, params.border);
                matcher.insert(std::pair<ImageTile, Tile>(imageTile, tile));
            }
            else if (offsets.size() > 1 && !disjoint_error)
            {
                WarnLog("Tiles found in tileset image %s are not disjoint, offset calculations may be off", image.name.c_str());
                disjoint_error = true;
            }
        }
    }

    // Checks
    int tile_size = TILE_SIZE_BYTES_8BPP;
    int memory_b = tiles.size() * tile_size;
    if (tiles.size() >= 1024)
        FatalLog("Too many tiles found %d tiles. Please make the image simpler.", tiles.size());


    // Delicious infos
    int cbbs = tiles.size() * tile_size / SIZE_CBB_BYTES;
    int sbbs = (int) ceil(tiles.size() * tile_size % SIZE_CBB_BYTES / ((double)SIZE_SBB_BYTES));
    InfoLog("Tiles found %zu.", tiles.size());
    InfoLog("Tiles uses %d charblocks and %d screenblocks.", cbbs, sbbs);
    InfoLog("Total utilization %.2f/4 charblocks or %d/32 screenblocks, %d/65536 bytes.",
        memory_b / ((double)SIZE_CBB_BYTES), (int) ceil(memory_b / ((double)SIZE_SBB_BYTES)), memory_b);
}

void Tileset::Init16bpp(const std::vector<Image16Bpp>& images)
{
    int tile_width = 8 + params.border;
    const ImageTile& nullTile = ImageTile::GetNullTile();
    itiles.insert(nullTile);

    for (unsigned int k = 0; k < images.size(); k++)
    {
        const Image16Bpp& image = images[k];

        unsigned int tilesX = image.width / tile_width;
        unsigned int tilesY = image.height / tile_width;
        unsigned int totalTiles = tilesX * tilesY;

        // Perform reduce.
        for (unsigned int i = 0; i < totalTiles; i++)
        {
            int tilex = i % tilesX;
            int tiley = i / tilesX;
            ImageTile tile(image, tilex, tiley, params.border);
            std::set<ImageTile>::iterator foundTile = itiles.find(tile);
            if (foundTile == itiles.end())
            {
                tile.id = itiles.size();
                itiles.insert(tile);
            }
        }
    }
}

Map::Map(const Image16Bpp& image, int bpp) : Image(image.width / 8, image.height / 8, image.name, image.filename, image.frame, image.animated),
    data(width * height), tileset(NULL), export_shared_info(true)
{
    // Create tileset according to bpp
    tileset.reset(new Tileset(image, bpp));

    // Tile match each tile in image
    switch(bpp)
    {
        case 4:
            Init4bpp(image);
            break;
        default:
            Init8bpp(image);
            break;
    }
}

Map::Map(const Image16Bpp& image, std::shared_ptr<Tileset>& global_tileset) : Image(image.width / 8, image.height / 8, image.name, image.filename, image.frame, image.animated),
    data(width * height), tileset(global_tileset), export_shared_info(false)
{
    switch(tileset->bpp)
    {
        case 4:
            Init4bpp(image);
            break;
        default:
            Init8bpp(image);
            break;
    }
}

void Map::Init4bpp(const Image16Bpp& image)
{
    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;
        ImageTile imageTile(image, tilex, tiley);
        int tile_id = 0;
        int pal_id = 0;

        if (!tileset->Match(imageTile, tile_id, pal_id))
        {
            WarnLog("Image: %s No match for tile starting at (%d %d) px, using empty tile instead.", image.name.c_str(), tilex * 8, tiley * 8);
            WarnLog("Image: %s No match for palette for tile starting at (%d %d) px, using palette 0 instead.", image.name.c_str(), tilex * 8, tiley * 8);
        }
        data[i] = pal_id << 12 | tile_id;
    }
}

void Map::Init8bpp(const Image16Bpp& image)
{
    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;
        ImageTile tile(image, tilex, tiley);
        int tile_id = 0;
        int pal_id = 0;

        if (!tileset->Match(tile, tile_id, pal_id))
            WarnLog("Image: %s No match for tile starting at (%d %d) px, using empty tile instead.", image.name.c_str(), tilex * 8, tiley * 8);

        data[i] = tile_id;
    }
}

void Map::WriteData(std::ostream& file) const
{
    // Sole owner of tileset.
    if (export_shared_info)
        tileset->WriteData(file);

    char buffer[7];
    int type = (width > 32 ? 1 : 0) | (height > 32 ? 1 : 0) << 1;
    int num_blocks = (type == 0 ? 1 : (type < 3 ? 2 : 4));

    WriteBeginArray(file, "const unsigned short", export_name, "_map", num_blocks * 32 * 32);
    for (int i = 0; i < num_blocks; i++)
    {
        // Case for each possible value of num_blocks
        // 1: 0
        // 2: type is 1 - 0, 1
        //    type is 2 - 0, 2
        // 4: 0, 1, 2, 3
        int sbb = (i == 0 ? 0 : (i == 1 && type == 2 ? 2 : i));
        unsigned int sx, sy;
        sx = ((sbb & 1) != 0) * 32;
        sy = ((sbb & 2) != 0) * 32;
        for (unsigned int y = 0; y < 32; y++)
        {
            for (unsigned int x = 0; x < 32; x++)
            {
                // Read tile if outside bounds replace with null tile
                unsigned short tile_id;
                if (x + sx > width || y + sy > height)
                    tile_id = 0;
                else
                    tile_id = data[(y + sy) * width + (x + sx)];

                snprintf(buffer, 7, "0x%04x", tile_id);
                // Write it.
                WriteElement(file, buffer, num_blocks * 32 * 32, (y + sy) * width + (x + sx), 8);
            }
        }
    }
    WriteEndArray(file);
    WriteNewLine(file);
}

void Map::WriteCommonExport(std::ostream& file) const
{
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
    WriteDefine(file, name, "_MAP_SIZE", data.size());
    WriteDefine(file, name, "_MAP_TYPE", (width > 32 ? 1 : 0) | (height > 32 ? 1 : 0) << 1, 14);
}

void Map::WriteExport(std::ostream& file) const
{
    // Sole owner of tileset.
    if (export_shared_info)
        tileset->WriteExport(file);

    WriteExtern(file, "const unsigned short", export_name, "_map", data.size());
    if (animated)
    {
        WriteDefine(file, export_name, "_WIDTH", width);
        WriteDefine(file, export_name, "_HEIGHT", height);
        WriteDefine(file, export_name, "_MAP_SIZE", data.size());
        WriteDefine(file, export_name, "_MAP_TYPE", (width > 32 ? 1 : 0) | (height > 32 ? 1 : 0) << 1, 14);
    }
    WriteNewLine(file);
}

MapScene::MapScene(const std::vector<Image16Bpp>& images16, const std::string& _name, int bpp) : Scene(_name), tileset(NULL)
{
    tileset.reset(new Tileset(images16, name, bpp));

    for (const auto& image : images16)
        images.emplace_back(new Map(image, tileset));
}

MapScene::MapScene(const std::vector<Image16Bpp>& images16, const std::string& _name, std::shared_ptr<Tileset>& _tileset) : Scene(_name), tileset(_tileset)
{
    for (const auto& image : images16)
        images.emplace_back(new Map(image, tileset));
}

void MapScene::WriteData(std::ostream& file) const
{
    tileset->WriteData(file);
    Scene::WriteData(file);
}

void MapScene::WriteExport(std::ostream& file) const
{
    tileset->WriteExport(file);
    Scene::WriteExport(file);
}

Sprite::Sprite(const Image16Bpp& image, std::shared_ptr<Palette>& global_palette) :
    Image(image.width / 8, image.height / 8, image.name, image.filename, image.frame, image.animated), palette(global_palette),
    palette_bank(-1), size(0), shape(0), offset(0)
{
    unsigned int key = (log2(width) << 2) | log2(height);
    shape = sprite_shapes[key];
    size = sprite_sizes[key];
    if (size == -1 || image.width & 7 || image.height & 7)
        FatalLog("Invalid sprite size, (%d %d) Please fix", image.width, image.height);

    data.reserve(width * height);
    for (unsigned int i = 0; i < width * height; i++)
        data.emplace_back(image, global_palette, i % width, i / width);
}

Sprite::Sprite(const Image16Bpp& image, int bpp) : Image(image.width / 8, image.height / 8, image.name, image.filename, image.frame, image.animated),
    palette(new Palette()), palette_bank(-1), size(0), shape(0), offset(0)
{
    unsigned int key = (log2(width) << 2) | log2(height);
    shape = sprite_shapes[key];
    size = sprite_sizes[key];
    if (size == -1 || image.width & 7 || image.height & 7)
        FatalLog("Invalid sprite size, (%d %d) Please fix", image.width, image.height);

    GetPalette(image.pixels, 0, params.transparent_color, 0, *palette);
    // Is actually an 8 or 4bpp image
    Image8Bpp image8(image, palette);

    data.reserve(width * height);
    for (unsigned int i = 0; i < width * height; i++)
        data.emplace_back(image8, i % width, i / width, 0, bpp);
}

void Sprite::UsePalette(const PaletteBank& bank)
{
    for (auto& tile : data)
        tile.UsePalette(bank);
    palette->Set(bank.GetColors());
}

void Sprite::WriteTile(unsigned char* arr, int x, int y) const
{
    int index = y * width + x;
    const Tile& tile = data[index];
    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        arr[i] = tile.pixels[i];
    }
}

void Sprite::WriteCommonExport(std::ostream& file) const
{
    WriteDefine(file, name, "_SHAPE", shape, 14);
    WriteDefine(file, name, "_SIZE", size, 14);
}

void Sprite::WriteExport(std::ostream& file) const
{
    if (params.bpp == 4)
        WriteDefine(file, export_name, "_PALETTE", palette_bank, 12);
    if (!animated)
    {
        WriteDefine(file, export_name, "_SPRITE_SHAPE", shape, 14);
        WriteDefine(file, export_name, "_SPRITE_SIZE", size, 14);
    }
    WriteDefine(file, export_name, "_ID", offset | (params.for_bitmap ? 512 : 0));
    WriteNewLine(file);
}

std::string Sprite::GetExportName() const
{
    return ToUpper(export_name) + "_ID";
}

std::ostream& operator<<(std::ostream& file, const Sprite& sprite)
{
    for (unsigned int i = 0; i < sprite.data.size(); i++)
    {
        file << sprite.data[i];
        if (i != sprite.data.size() - 1)
            file << ",\n\t";
    }
    return file;
}

bool BlockSize::operator==(const BlockSize& rhs) const
{
    return width == rhs.width && height == rhs.height;
}

bool BlockSize::operator<(const BlockSize& rhs) const
{
    if (width != rhs.width)
        return width < rhs.width;
    else
        return height < rhs.height;
}

std::vector<BlockSize> BlockSize::BiggerSizes(const BlockSize& b)
{
    switch(b.Size())
    {
    case 1:
        return {BlockSize(2, 1), BlockSize(1, 2)};
    case 2:
        if (b.width == 2)
            return {BlockSize(4, 1), BlockSize(2, 2)};
        else
            return {BlockSize(1, 4), BlockSize(2, 2)};
    case 4:
        if (b.width == 4)
            return {BlockSize(4, 2)};
        else if (b.height == 4)
            return {BlockSize(2, 4)};
        else
            return {BlockSize(4, 2), BlockSize(2, 4)};
    case 8:
        return {BlockSize(4, 4)};
    case 16:
        return {BlockSize(8, 4), BlockSize(4, 8)};
    case 32:
        return {BlockSize(8, 8)};
    default:
        return {};
    }
}

Block Block::VSplit()
{
    size.height /= 2;
    return Block(x, y + size.height, size);
}

Block Block::HSplit()
{
    size.width /= 2;
    return Block(x + size.width, y, size);
}

Block Block::Split(const BlockSize& to_this_size)
{
    if (size.width == to_this_size.width)
        return VSplit();
    else if (size.height == to_this_size.height)
        return HSplit();
    else
        FatalLog("Error Block::Split this shouldn't happen");

    return Block();
}

bool SpriteCompare(const Image* l, const Image* r)
{
    const Sprite* lhs = dynamic_cast<const Sprite*>(l);
    const Sprite* rhs = dynamic_cast<const Sprite*>(r);
    if (lhs->Size() != rhs->Size())
        return lhs->Size() > rhs->Size();
    else
        // Special case 2x2 should be of lesser priority than 4x1/1x4
        // This is because 2x2's don't care how a 4x4 block is split they are happy with a 4x2 or 2x4
        // Whereas 4x1 must get 4x2 and 1x4 must get 2x4.
        return lhs->width + lhs->height > rhs->width + rhs->height;
}

SpriteSheet::SpriteSheet(const std::vector<Sprite*>& _sprites, const std::string& _name, int _bpp, bool _spriteSheetGiven) :
    name(_name), bpp(_bpp), spriteSheetGiven(_spriteSheetGiven), sprites(_sprites)
{
    width = bpp == 4 ? 32 : 16;
    height = !params.for_bitmap ? 32 : 16;
    data.resize(width * 8 * height * 8);
}

void SpriteSheet::Compile()
{
    PlaceSprites();
    for (const auto& block : placedBlocks)
    {
        for (unsigned int i = 0; i < block.size.height; i++)
        {
            for (unsigned int j = 0; j < block.size.width; j++)
            {
                int x = block.x + j;
                int y = block.y + i;
                int index = y * width + x;
                const Sprite* sprite = sprites[block.sprite_id];
                sprite->WriteTile(data.data() + index * 64, j, i);
            }
        }
    }
}

void SpriteSheet::WriteData(std::ostream& file) const
{
    if (bpp == 8)
        WriteShortArray(file, name, "", data, 8);
    else
        WriteShortArray4Bit(file, name, "", data, 8);
    WriteNewLine(file);
}

void SpriteSheet::WriteExport(std::ostream& file) const
{
    unsigned int size = data.size() / (bpp == 4 ? 4 : 2);
    WriteExtern(file, "const unsigned short", name, "", size);
    WriteDefine(file, name, "_SIZE", size);
    WriteNewLine(file);

    if (!spriteSheetGiven)
    {
        for (const auto& sprite : sprites)
            sprite->WriteExport(file);
    }
}

void SpriteSheet::PlaceSprites()
{
    if (spriteSheetGiven)
    {
        for (unsigned int i = 0; i < sprites.size(); i++)
        {
            Sprite& sprite = *sprites[i];
            Block allocd(i % width, i / width, BlockSize(1, 1));
            allocd.sprite_id = i;
            sprite.offset = (allocd.y * width + allocd.x) * (params.bpp == 4 ? 1 : 2);
            placedBlocks.push_back(allocd);
        }
        return;
    }

    // Gimme some blocks.
    BlockSize size(8, 8);
    for (unsigned int y = 0; y < height; y += 8)
    {
        for (unsigned int x = 0; x < width; x += 8)
        {
            freeBlocks[size].push_back(Block(x, y, size));
        }
    }

    // Sort by request size
    std::sort(sprites.begin(), sprites.end(), SpriteCompare);

    for (unsigned int i = 0; i < sprites.size(); i++)
    {
        Sprite& sprite = *sprites[i];
        BlockSize size(sprite.width, sprite.height);
        std::list<BlockSize> slice;

        // Mother may I have block of this size?
        if (AssignBlockIfAvailable(size, sprite, i))
            continue;

        if (size.IsBiggestSize())
            FatalLog("Out of sprite memory could not allocate sprite %s size (%d,%d)", sprite.name.c_str(), sprite.width, sprite.height);

        slice.push_front(size);
        while (!HasAvailableBlock(size))
        {
            std::vector<BlockSize> sizes = BlockSize::BiggerSizes(size);
            if (sizes.empty())
                FatalLog("Out of sprite memory could not allocate sprite %s size (%d,%d)", sprite.name.c_str(), sprite.width, sprite.height);

            // Default next search size will be last.
            size = sizes.back();
            for (auto& new_size : sizes)
            {
                if (HasAvailableBlock(new_size))
                {
                    size = new_size;
                    break;
                }
            }
            if (!HasAvailableBlock(size))
                slice.push_front(size);
        }

        if (!HasAvailableBlock(size))
            FatalLog("Out of sprite memory could not allocate sprite %s size (%d,%d)", sprite.name.c_str(), sprite.width, sprite.height);

        SliceBlock(size, slice);

        size = BlockSize(sprite.width, sprite.height);
        // Mother may I have block of this size?
        if (AssignBlockIfAvailable(size, sprite, i))
            continue;
        else
            FatalLog("Out of sprite memory could not allocate sprite %s size (%d,%d)", sprite.name.c_str(), sprite.width, sprite.height);
    }
}

bool SpriteSheet::AssignBlockIfAvailable(BlockSize& size, Sprite& sprite, unsigned int i)
{
    if (HasAvailableBlock(size))
    {
        // Yes you may deary.
        Block allocd = freeBlocks[size].front();
        freeBlocks[size].pop_front();
        allocd.sprite_id = i;
        sprite.offset = (allocd.y * width + allocd.x) * (params.bpp == 4 ? 1 : 2);
        placedBlocks.push_back(allocd);
        return true;
    }

    return false;
}

bool SpriteSheet::HasAvailableBlock(const BlockSize& size)
{
    return !freeBlocks[size].empty();
}

void SpriteSheet::SliceBlock(const BlockSize& size, const std::list<BlockSize>& slice)
{
    Block toSplit = freeBlocks[size].front();
    freeBlocks[size].pop_front();

    for (const auto& split_size : slice)
    {
        Block other = toSplit.Split(split_size);
        freeBlocks[split_size].push_back(other);
    }

    freeBlocks[toSplit.size].push_front(toSplit);
}

SpriteScene::SpriteScene(const std::vector<Image16Bpp>& images, const std::string& _name, bool _is2d, int _bpp) : Scene(_name), bpp(_bpp), paletteBanks(name),
    is2d(_is2d), spriteSheetGiven(false)
{
    switch(bpp)
    {
        case 4:
            Init4bpp(images);
            break;
        default:
            Init8bpp(images);
            break;
    }
}

SpriteScene::SpriteScene(const std::vector<Image16Bpp>& images, const std::string& _name, bool _is2d, std::shared_ptr<Palette>& _palette) :
    Scene(_name), bpp(8), palette(_palette), paletteBanks(name), is2d(_is2d), spriteSheetGiven(false)
{
    Init8bpp(images);
}

SpriteScene::SpriteScene(const std::vector<Image16Bpp>& images, const std::string& _name, bool _is2d, const std::vector<PaletteBank>& _paletteBanks) :
    Scene(_name), bpp(4), paletteBanks(name, _paletteBanks), is2d(_is2d), spriteSheetGiven(false)
{
    Init4bpp(images);
}

void SpriteScene::Build()
{
    if (is2d)
    {
        std::vector<Sprite*> sprites;
        for (const auto& image : images)
        {
            Sprite* sprite = dynamic_cast<Sprite*>(image.get());
            if (!sprite) FatalLog("Could not cast Image to Sprite. This shouldn't happen");
            sprites.push_back(sprite);
        }
        spriteSheet.reset(new SpriteSheet(sprites, name, bpp, spriteSheetGiven));
        spriteSheet->Compile();
    }
    else
    {
        unsigned int offset = 0;
        for (unsigned int k = 0; k < images.size(); k++)
        {
            Sprite* sprite = dynamic_cast<Sprite*>(images[k].get());
            if (!sprite) FatalLog("Could not cast Image to Sprite. This shouldn't happen");
            sprite->offset = offset;
            offset += sprite->width * sprite->height * (bpp == 8 ? 2 : 1);
        }
    }
}

const Sprite& SpriteScene::GetSprite(int index) const
{
    const Sprite* sprite = dynamic_cast<const Sprite*>(images[index].get());
    if (!sprite) FatalLog("Could not cast Image to Sprite. This shouldn't happen");
    return *sprite;
}

unsigned int SpriteScene::Size() const
{
    unsigned int total = 0;
    for (const auto& image : images)
        total += image->width * image->height;

    return total * (bpp == 4 ? TILE_SIZE_SHORTS_4BPP : TILE_SIZE_SHORTS_8BPP);
}

void SpriteScene::WriteData(std::ostream& file) const
{
    if (bpp == 4)
        paletteBanks.WriteData(file);
    else
        palette->WriteData(file);

    if (is2d)
    {
        spriteSheet->WriteData(file);
    }
    else
    {
        WriteBeginArray(file, "const unsigned short", name, "", Size());
        for (unsigned int i = 0; i < images.size(); i++)
        {
            file << GetSprite(i);
            if (i != images.size() - 1)
                file << ",\n\t";
        }
        WriteEndArray(file);
        WriteNewLine(file);
    }
}

void SpriteScene::WriteExport(std::ostream& file) const
{
    WriteDefine(file, name, "_PALETTE_TYPE", bpp == 8, 13);
    WriteDefine(file, name, "_DIMENSION_TYPE", !is2d, 6);
    WriteNewLine(file);

    if (bpp == 4)
        paletteBanks.WriteExport(file);
    else
        palette->WriteExport(file);


    if (is2d)
    {
        spriteSheet->WriteExport(file);
    }
    else
    {
        WriteExtern(file, "const unsigned short", name, "", Size());
        WriteDefine(file, name, "_SIZE", Size());
        WriteNewLine(file);

        if (!spriteSheetGiven)
        {
            for (const auto& sprite : images)
                sprite->WriteExport(file);
        }
    }
}

bool SpritePaletteSizeComp(const Sprite* i, const Sprite* j)
{
    return i->palette->Size() > j->palette->Size();
}

void SpriteScene::Init4bpp(const std::vector<Image16Bpp>& images16)
{
    // Form sprites
    std::vector<Sprite*> sprites;
    for (const auto& image : images16)
        sprites.push_back(new Sprite(image, bpp));

    // Palette bank selection time
    // Ensure image contains < 256 colors
    std::set<Color> bigPalette;
    for (const auto& sprite : sprites)
    {
        const std::vector<Color>& sprite_palette = sprite->palette->GetColors();
        bigPalette.insert(sprite_palette.begin(), sprite_palette.end());
    }

    if (bigPalette.size() > 256 && !params.force)
        FatalLog("Image after reducing sprites to 4bpp still contains more than 256 distinct colors.  Found %d colors. Please fix.", bigPalette.size());

    // Greedy approach deal with tiles with largest palettes first.
    std::sort(sprites.begin(), sprites.end(), SpritePaletteSizeComp);

    // But deal with transparent color
    for (unsigned int i = 0; i < paletteBanks.Size(); i++)
        paletteBanks[i].Add(Color(params.transparent_color));

    // Construct palette banks, assign bank id to tile, remap sprite to palette bank given, assign tile ids
    for (auto& sprite : sprites)
    {
        int pbank = -1;
        // Fully contains checks
        for (unsigned int i = 0; i < paletteBanks.Size(); i++)
        {
            PaletteBank& bank = paletteBanks[i];
            if (bank.Contains(*sprite->palette))
                pbank = i;
        }

        // Ok then find least affected bank
        if (pbank == -1)
        {
            int max_colors_left = -1;
            for (unsigned int i = 0; i < paletteBanks.Size(); i++)
            {
                PaletteBank& bank = paletteBanks[i];
                int colors_left = bank.CanMerge(*sprite->palette);
                if (colors_left != 0 && max_colors_left < colors_left)
                {
                    max_colors_left = colors_left;
                    pbank = i;
                }
            }
        }
        // Cry and die for now. Unless you tell me to keep going.
        if (pbank == -1 && !params.force)
            FatalLog("More than 16 distinct palettes found, please use 8bpp mode.");

        // Merge step and assign palette bank
        paletteBanks[pbank].Merge(*sprite->palette);
        sprite->palette_bank = pbank;
        sprite->UsePalette(paletteBanks[pbank]);

        // Add to images list
        images.emplace_back(sprite);
    }
}

void SpriteScene::Init8bpp(const std::vector<Image16Bpp>& images16)
{
    if (!palette)
    {
        Image8BppScene scene(images16, name);
        palette = scene.palette;
    }

    for (const auto& image : images16)
        images.emplace_back(new Sprite(image, palette));
}

void GetPalette(const std::vector<Color>& image, unsigned int num_colors, const Color& transparent, unsigned int offset, Palette& palette)
{
    std::vector<Color> pixels;
    for (const auto& color : image)
    {
        if (color != transparent)
            pixels.push_back(color);
    }

    std::vector<Color> paletteArray;
    if (!offset)
        paletteArray.push_back(transparent);

    MedianCut(pixels, num_colors - 1, paletteArray, params.weights.data());
    palette.Set(paletteArray);
}

void QuantizeImage(const std::vector<Color>& image, unsigned int num_colors, const Color& transparent, unsigned int offset, Palette& palette, std::vector<unsigned char>& indexed)
{
    GetPalette(image, num_colors, transparent, offset, palette);
    for (const auto& color : image)
        indexed.push_back((color == transparent) ? 0 : palette.Search(color) + offset);
}

void DitherAndReduceImage(const Image16Bpp& image, const Color& transparent, bool dither, double dither_level, unsigned int offset, Image8Bpp& indexedImage)
{
    RiemersmaDither(image, indexedImage, transparent, dither, dither_level);
    if (offset > 0)
    {
        for (unsigned char& pix : indexedImage.pixels)
            if (pix)
                pix += offset;
    }
}
