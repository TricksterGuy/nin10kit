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
//h =  1,  2, 4,  8
    0,  2, 2, -1, // width = 1
    1,  0, 2, -1, // width = 2
    1,  1, 0,  2, // width = 4
    -1, -1, 1,  0  // width = 8
};

const int sprite_sizes[16] =
{
//h =  1,  2, 4,  8
    0,  0, 1, -1, // width = 1
    0,  1, 2, -1, // width = 2
    1,  2, 2,  3, // width = 4
    -1, -1, 3,  3  // width = 8
};

Image32Bpp::Image32Bpp(const Magick::Image& image, const std::string& name, const std::string& filename, unsigned int frame, bool animated) :
    Image(image.columns(), image.rows(), name, filename, frame, animated), has_alpha(false), pixels(3 * width * height)
{
    unsigned int num_pixels = width * height;
    const Magick::PixelPacket* imageData = image.getConstPixels(0, 0, image.columns(), image.rows());

    size_t depth;
    MagickCore::GetMagickQuantumDepth(&depth);
    for (unsigned int i = 0; i < num_pixels; i++)
    {
        const Magick::PixelPacket& packet = imageData[i];
        int r, g, b;
        if (depth == 8)
        {
            r = packet.red;
            g = packet.green;
            b = packet.blue;
        }
        else if (depth == 16)
        {
            r = (packet.red >> 8) & 0xFF;
            g = (packet.green >> 8) & 0xFF;
            b = (packet.blue >> 8) & 0xFF;
        }
        else
        {
            // To get rid of warning
            b = g = r = 0;
            FatalLog("Image quantum not supported");
        }

        pixels[3 * i] = b;
        pixels[3 * i + 1] = g;
        pixels[3 * i + 2] = r;
    }
}

void Image32Bpp::WriteData(std::ostream& file) const
{
    WriteCharArray(file, export_name, "", pixels, 10);
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
    const std::vector<unsigned char>& pixels32 = image.pixels;

    for (unsigned int i = 0; i < num_pixels; i++)
    {
        char r, g, b;
        r = (pixels32[3 * i + 2] >> 3) & 0x1F;
        g = (pixels32[3 * i + 1] >> 3) & 0x1F;
        b = (pixels32[3 * i] >> 3) & 0x1F;
        pixels[i] = r | g << 5 | b << 10;
    }
}

void Image16Bpp::GetColors(std::vector<Color>& colors) const
{
    for (unsigned int i = 0; i < width * height; i++)
    {
        short pix = pixels[i];
        if (pix != params.transparent_color)
        {
            colors.push_back(Color(pix));
        }
    }
}

Image16Bpp Image16Bpp::SubImage(unsigned int x, unsigned int y, unsigned int swidth, unsigned int sheight) const
{
    Image16Bpp sub(swidth, sheight);
    for (unsigned int i = 0; i < sheight; i++)
    {
        for (unsigned int j = 0; j < swidth; j++)
        {
            sub.pixels[i * swidth + j] = pixels[(i + y) * width + j + x];
        }
    }
    return sub;
}

void Image16Bpp::WriteData(std::ostream& file) const
{
    WriteShortArray(file, export_name, "", pixels, 10);
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
    register double bestd = DBL_MAX;
    int index = -1;

    if (colorIndexCache.find(a) != colorIndexCache.end())
        return colorIndexCache[a];

    for (unsigned int i = 0; i < colors.size(); i++)
    {
        double dist = 0;
        const Color& b = colors[i];
        dist = a.Distance(b);
        if (dist < bestd)
        {
            index = i;
            bestd = dist;
        }
    }

    colorIndexCache[a] = index;

    if (bestd != 0)
    {
        int x, y, z;
        a.Get(x, y, z);
        unsigned short c = x | (y << 5) | (z << 10);
        VerboseLog("Color remap: Color %d given to palette bank not an exact match.", c);
    }

    return index;
}

int ColorArray::Search(unsigned short color_data) const
{
    return Search(Color(color_data));
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

Palette::Palette(const std::vector<Color>& colors, const std::string& _name) : ColorArray(colors), name(_name)
{
    if (colors.size() + params.offset > 256)
        FatalLog("Too many colors in palette. Found %d colors, offset is %d.", colors.size() + params.offset, params.offset);
}

void Palette::WriteData(std::ostream& file) const
{
    WriteShortArray(file, name, "_palette", colors, 10);
    WriteNewLine(file);
}

void Palette::WriteExport(std::ostream& file) const
{
    WriteExtern(file, "const unsigned short", name, "_palette", colors.size());
    WriteDefine(file, name, "_PALETTE_SIZE", colors.size());
    WriteNewLine(file);
}

Image8Bpp::Image8Bpp(const Image16Bpp& image) : Image(image), pixels(width * height), palette(NULL), export_shared_info(true)
{
    // If the image width is odd error out
    if (width & 1)
        FatalLog("Image: %s width is not a multiple of 2. Please fix", name.c_str());

    std::vector<Color> pixels16;
    image.GetColors(pixels16);

    // If transparent color is present add to palette, but only if offset is not 0
    int palette_size = params.palette;
    std::vector<Color> paletteColors(palette_size);
    if (params.offset == 0)
    {
        palette_size--;
        paletteColors.push_back(Color(params.transparent_color));
    }

    MedianCut(pixels16, palette_size, paletteColors, params.weights.data());

    palette.reset(new Palette(paletteColors, export_name));
    RiemersmaDither(image, *this, params.transparent_color, params.dither, params.dither_level);
    if (params.offset > 0)
    {
        for (unsigned char& pix : pixels)
            if (pix)
                pix += params.offset;
    }
}

Image8Bpp::Image8Bpp(const Image16Bpp& image, std::shared_ptr<Palette>& global_palette) : Image(image), pixels(width * height), palette(global_palette), export_shared_info(false)
{
    if (width & 1)
        FatalLog("Image: %s width is not a multiple of 2. Please fix", name.c_str());

    RiemersmaDither(image, *this, params.transparent_color, params.dither, params.dither_level);
    if (params.offset > 0)
    {
        for (unsigned char& pix : pixels)
            if (pix)
                pix += params.offset;
    }
}

void Image8Bpp::WriteData(std::ostream& file) const
{
    // Sole owner of palette
    if (export_shared_info)
        palette->WriteData(file);
    WriteShortArray(file, export_name, "", pixels, 10);
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

Image8BppScene::Image8BppScene(const std::vector<Image16Bpp>& images16, const std::string& _name) : Scene(_name), palette(NULL)
{
    unsigned int total_pixels = 0;
    for (unsigned int i = 0; i < images16.size(); i++)
        total_pixels += images16[i].width * images16[i].height;

    std::vector<Color> pixels(total_pixels);
    for (unsigned int i = 0; i < images16.size(); i++)
        images16[i].GetColors(pixels);

    // If transparent color is present add to palette
    std::vector<Color> paletteColors;
    paletteColors.reserve(params.palette);
    if (params.offset == 0)
    {
        params.palette -= 1;
        paletteColors.push_back(Color(params.transparent_color));
    }

    MedianCut(pixels, params.palette, paletteColors, params.weights.data());
    palette.reset(new Palette(paletteColors, name));

    images.reserve(images16.size());
    for (unsigned int i = 0; i < images.size(); i++)
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
    for (const auto& color : palette.colors)
        tmpset.insert(color);

    return 16 - tmpset.size();
}

void PaletteBank::Merge(const ColorArray& palette)
{
    for (const auto& color : palette.colors)
        Add(color);
}

std::ostream& operator<<(std::ostream& file, const PaletteBank& bank)
{
    char buffer[7];
    std::vector<Color> colors = bank.colors;
    colors.resize(16);
    for (unsigned int i = 0; i < colors.size(); i++)
    {
        int x, y, z;
        const Color& color = colors[i];
        color.Get(x, y, z);
        unsigned short data_read = x | (y << 5) | (z << 10);
        snprintf(buffer, 7, "0x%04x", data_read);
        WriteElement(file, buffer, colors.size(), i, 8);
    }

    return file;
}

PaletteBankManager::PaletteBankManager(const std::string& _name) : name(_name), banks(16)
{
    for (unsigned int i = 0; i < banks.size(); i++)
        banks[i].id = i;
}

PaletteBankManager::PaletteBankManager(const std::string& _name, const std::vector<PaletteBank>& paletteBanks) : name(_name), banks(paletteBanks)
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

template <>
Tile<unsigned short>::Tile() : id(0), data(TILE_SIZE), bpp(16), palette_bank(0)
{
}

template <>
Tile<unsigned short>::Tile(const std::vector<unsigned short>& image, int pitch, int tilex, int tiley, int border, int ignored) : data(TILE_SIZE), palette_bank(0)
{
    Set(image, pitch, tilex, tiley, border, 16);
}

template <>
Tile<unsigned short>::Tile(const unsigned short* image, int pitch, int tilex, int tiley, int border, int ignored) : data(TILE_SIZE), palette_bank(0)
{
    Set(image, pitch, tilex, tiley, border, 16);
}

template <>
Tile<unsigned char>::Tile(std::shared_ptr<ImageTile>& imageTile, int _bpp) : data(TILE_SIZE), bpp(_bpp)
{
    // bpp reduce minus one for the transparent color
    int num_colors = (1 << bpp) - 1;
    const unsigned short* imgdata = imageTile->data.data();
    int weights[4] = {25, 25, 25, 25};

    std::vector<Color> pixels;
    pixels.reserve(TILE_SIZE);

    std::vector<Color> paletteArray;
    paletteArray.reserve(1 << bpp);

    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        unsigned short pix = imgdata[i];
        if (pix != params.transparent_color)
            pixels.push_back(Color(pix));
    }
    paletteArray.push_back(Color(params.transparent_color));
    MedianCut(pixels, num_colors, paletteArray, weights);

    palette.Set(paletteArray);

    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        unsigned short pix = imgdata[i];
        data[i] = (pix != params.transparent_color) ? palette.Search(pix) : 0;
    }

    sourceTile = imageTile;
}

template <>
void Tile<unsigned char>::Set(const Image16Bpp& image, const Palette& global_palette, int tilex, int tiley)
{
    bpp = 8;
    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        int x = i % 8;
        int y = i / 8;
        data[i] = global_palette.Search(image.pixels[(tiley * 8 + y) * image.width + tilex * 8 + x]);
    }
}

template <>
void Tile<unsigned char>::UsePalette(const PaletteBank& bank)
{
    if (bpp == 8)
        FatalLog("Code error not implemented");

    std::map<int, int> remapping;
    for (unsigned int i = 0; i < palette.Size(); i++)
    {
        Color old = palette.At(i);
        remapping[i] = bank.Search(old);
    }

    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        if (remapping.find(data[i]) == remapping.end())
            FatalLog("Somehow tile contains invalid indicies. This shouldn't happen");

        data[i] = remapping[data[i]];
    }

    palette.Set(bank.colors);
}

template <>
std::ostream& operator<<(std::ostream& file, const Tile<unsigned char>& tile)
{
    char buffer[7];
    const std::vector<unsigned char>& data = tile.data;
    if (tile.bpp == 8)
    {
        for (unsigned int i = 0; i < TILE_SIZE_SHORTS_8BPP; i++)
        {
            snprintf(buffer, 7, "0x%02x%02x", data[2 * i + 1], data[2 * i]);
            WriteElement(file, buffer, TILE_SIZE_SHORTS_8BPP, i, 8);
        }
    }
    else
    {
        for (unsigned int i = 0; i < TILE_SIZE_SHORTS_4BPP; i++)
        {
            snprintf(buffer, 7, "0x%01x%01x%01x%01x", data[4 * i + 3], data[4 * i + 2], data[4 * i + 1], data[4 * i]);
            WriteElement(file, buffer, TILE_SIZE_SHORTS_4BPP, i, 8);
        }
    }
    return file;
}

bool TilesPaletteSizeComp(const GBATile& i, const GBATile& j)
{
    return i.palette.Size() > j.palette.Size();
}

Tileset::Tileset(const std::vector<Image16Bpp>& images, const std::string& _name, int _bpp) : name(_name), bpp(_bpp), paletteBanks(name)
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

Tileset::Tileset(const Image16Bpp& image, int _bpp) : name(image.name), bpp(_bpp), paletteBanks(name)
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

int Tileset::Search(const GBATile& tile) const
{
    const std::set<GBATile>::const_iterator foundTile = tiles.find(tile);
    if (foundTile != tiles.end())
    {
        std::cout << "found\n" << *foundTile << "\n";
        return foundTile->id;
    }

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
    const std::map<ImageTile, GBATile>::const_iterator foundTile = matcher.find(tile);
    if (foundTile != matcher.end())
    {
        const GBATile& tile = foundTile->second;
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

    std::vector<GBATile>::const_iterator tile_ptr = tilesExport.begin();
    file << "const unsigned short " << name << "_tiles[" << Size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < tilesExport.size(); i++)
    {
        file << *tile_ptr;
        if (i != tilesExport.size() - 1)
            file << ",\n\t";
        ++tile_ptr;
    }
    file << "\n};\n";
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

    GBATile nullTile;
    nullTile.id = 0;
    nullTile.bpp = 4;
    ImageTile nullImageTile;
    tiles.insert(nullTile);
    tilesExport.push_back(nullTile);
    matcher[nullImageTile] = nullTile;

    // Reduce each tile to 4bpp
    std::vector<GBATile> gbaTiles;
    for (std::set<ImageTile>::const_iterator i = imageTiles.begin(); i != imageTiles.end(); ++i)
    {
        std::shared_ptr<ImageTile> src(new ImageTile(*i));
        GBATile tile(src, 4);
        gbaTiles.push_back(tile);
    }

    // Ensure image contains < 256 colors
    std::set<Color> bigPalette;
    for (const auto& tile : gbaTiles)
    {
        const std::vector<Color>& tile_palette = tile.palette.colors;
        bigPalette.insert(tile_palette.begin(), tile_palette.end());
    }

    if (bigPalette.size() > 256 && !params.force)
        FatalLog("Image after reducing tiles to 4bpp still contains more than 256 distinct colors.  Found %d colors. Please fix.", bigPalette.size());

    // Greedy approach deal with tiles with largest palettes first.
    std::sort(gbaTiles.begin(), gbaTiles.end(), TilesPaletteSizeComp);

    // But deal with nulltile
    for (unsigned int i = 0; i < paletteBanks.Size(); i++)
        paletteBanks[i].Add(Color());
    nullTile.palette_bank = 0;


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
        std::set<GBATile>::const_iterator it = tiles.find(tile);
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

        // Form mapping from ImageTile to GBATile
        matcher[*tile.sourceTile] = tile;
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

    GBATile nullTile;
    ImageTile nullImageTile;
    tiles.insert(nullTile);
    tilesExport.push_back(nullTile);
    matcher[nullImageTile] = nullTile;

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
            GBATile tile(image.pixels, image.width, tilex, tiley, params.border);
            std::set<GBATile>::iterator foundTile = tiles.find(tile);
            if (foundTile == tiles.end())
            {
                tile.id = tiles.size();
                tiles.insert(tile);
                tilesExport.push_back(tile);
                // Add matcher data
                ImageTile imageTile(image16.pixels, image.width, tilex, tiley, params.border);
                matcher[imageTile] = tile;
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
    ImageTile nullTile;
    itiles.insert(nullTile);

    for (unsigned int k = 0; k < images.size(); k++)
    {
        const Image16Bpp& image = images[k];
        const std::vector<unsigned short>& pixels = image.pixels;

        unsigned int tilesX = image.width / tile_width;
        unsigned int tilesY = image.height / tile_width;
        unsigned int totalTiles = tilesX * tilesY;

        // Perform reduce.
        for (unsigned int i = 0; i < totalTiles; i++)
        {
            int tilex = i % tilesX;
            int tiley = i / tilesX;
            ImageTile tile(pixels, image.width, tilex, tiley, params.border);
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
    const std::vector<unsigned short>& pixels = image.pixels;

    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;
        ImageTile imageTile(pixels, image.width, tilex, tiley);
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
    const std::vector<unsigned short>& pixels = image.pixels;

    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;
        ImageTile tile(pixels, image.width, tilex, tiley);
        int tile_id = 0;
        int pal_id = 0;

        if (!tileset->Match(tile, tile_id, pal_id))
        {
            WarnLog("Image: %s No match for tile starting at (%d %d) px, using empty tile instead.", image.name.c_str(), tilex * 8, tiley * 8);
        }

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

    file << "const unsigned short " << export_name << "_map[" << num_blocks * 32 * 32 << "] =\n{\n\t";
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
    file << "\n};\n";
    WriteNewLine(file);
}

void Map::WriteCommonExport(std::ostream& file) const
{
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
    WriteDefine(file, name, "_MAP_SIZE", Size());
    WriteDefine(file, name, "_MAP_TYPE", Type(), 14);
}

void Map::WriteExport(std::ostream& file) const
{
    // Sole owner of tileset.
    if (export_shared_info)
        tileset->WriteExport(file);

    WriteExtern(file, "const unsigned short", export_name, "_map", Size());
    if (animated)
    {
        WriteDefine(file, export_name, "_WIDTH", width);
        WriteDefine(file, export_name, "_HEIGHT", height);
        WriteDefine(file, export_name, "_MAP_SIZE", Size());
        WriteDefine(file, export_name, "_MAP_TYPE", Type(), 14);
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
    Image(image.width / 8, image.height / 8, image.name, image.filename, image.frame, image.animated), data(width * height), palette(global_palette),
    palette_bank(-1), size(0), shape(0), offset(0)
{
    unsigned int key = (log2(width) << 2) | log2(height);
    shape = sprite_shapes[key];
    size = sprite_sizes[key];
    if (size == -1)
        FatalLog("Invalid sprite size, (%d %d) Please fix", width, height);

    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;

        data[i].Set(image, *global_palette, tilex, tiley);
    }
}

Sprite::Sprite(const Image16Bpp& image, int bpp) : Image(image.width / 8, image.height / 8, image.name, image.filename, image.frame, image.animated),
    data(width * height), palette_bank(-1), size(0), shape(0), offset(0)
{
    unsigned int key = (log2(width) << 2) | log2(height);
    shape = sprite_shapes[key];
    size = sprite_sizes[key];
    if (size == -1)
        FatalLog("Invalid sprite size, (%d %d) Please fix", width, height);

    // bpp reduce minus one for the transparent color
    int num_colors = (1 << bpp) - 1;
    const unsigned short* imgdata = image.pixels.data();
    unsigned int size = image.pixels.size();

    std::vector<Color> pixels(size);
    std::vector<Color> paletteArray(num_colors + 1);

    for (unsigned int i = 0; i < size; i++)
    {
        unsigned short pix = imgdata[i];
        if (pix != params.transparent_color)
            pixels.push_back(Color(pix));
    }
    paletteArray.push_back(Color(params.transparent_color));
    MedianCut(pixels, num_colors, paletteArray, params.weights.data());

    palette.reset(new Palette(paletteArray, ""));

    std::vector<unsigned char> data4bpp(size);
    for (unsigned int i = 0; i < size; i++)
    {
        unsigned short pix = imgdata[i];
        data4bpp[i] = (pix != params.transparent_color) ? palette->Search(pix) : 0;
    }

    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;

        data[i].Set(data4bpp, image.width, tilex, tiley, 0, 4);
        data[i].palette = *palette;
    }
}

void Sprite::UsePalette(const PaletteBank& bank)
{
    for (auto& tile : data)
        tile.UsePalette(bank);
    palette->Set(bank.colors);
}

void Sprite::WriteTile(unsigned char* arr, int x, int y) const
{
    int index = y * width + x;
    const GBATile& tile = data[index];
    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        arr[i] = tile.data[i];
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
        WriteDefine(file, export_name, "_SHAPE", shape, 14);
        WriteDefine(file, export_name, "_SIZE", size, 14);
    }
    WriteDefine(file, export_name, "_ID", offset | (params.for_bitmap ? 512 : 0));
    WriteNewLine(file);
}

void Sprite::WriteData(std::ostream& file) const
{
    // Implemented in operator<<
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

        if (size.isBiggestSize())
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
    if (images.size() == 1 && (images[0].width > 64 || images[0].height > 64))
    {
        spriteSheetGiven = true;
        is2d = !(images[0].width == 8 || images[0].height == 8);
        WarnLog("[WARNING] Spritesheet detected.\n"
                "If you formed your sprites in a single sprite sheet please note that \n"
                "the program will automatically build the spritesheet for you and export.\n"
                "Just pass in the images you want to use as sprites and let me do the rest.\n"
                "Override: using sprite mode %s.", (is2d ? "2D" : "1D"));
        InitSpriteSheet(images[0]);
        return;
    }

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
        file << "const unsigned short " << name << "[" << Size() << "] =\n{\n\t";
        for (unsigned int i = 0; i < images.size(); i++)
        {
            file << GetSprite(i);
            if (i != images.size() - 1)
                file << ",\n\t";
        }
        file << "\n};\n";
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
        const std::vector<Color>& sprite_palette = sprite->palette->colors;
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

void SpriteScene::InitSpriteSheet(const Image16Bpp& sheet)
{
    std::vector<Image16Bpp> tiles;
    int tilesX = sheet.width / 8;
    int tilesY = sheet.height / 8;
    tiles.reserve(tilesX * tilesY);
    for (int y = 0; y < tilesY; y++)
    {
        for (int x = 0; x < tilesX; x++)
        {
            tiles.push_back(sheet.SubImage(x * 8, y * 8, 8, 8));
        }
    }

    if (bpp == 4)
        Init4bpp(tiles);
    else
        Init8bpp(tiles);
}
