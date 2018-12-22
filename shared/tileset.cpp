#include "tileset.hpp"

#include <sstream>
#include "logger.hpp"
#include "export_params.hpp"
#include "fileutils.hpp"
#include "image16.hpp"
#include "image8.hpp"
#include "shared.hpp"

Tileset::Tileset(const std::vector<Image16Bpp>& images, const std::string& name, int _bpp, bool _affine, const std::shared_ptr<Palette>& global_palette) :
    Exportable(name), bpp(_bpp), affine(_affine), palette(global_palette), paletteBanks(name), export_shared_data(global_palette == nullptr)
{
    switch(bpp)
    {
        case 4:
            if (affine)
            {
                FatalLog("Affine can not be used with a 4 bpp tileset");
            }
            else if (palette)
            {
                paletteBanks.Copy(*global_palette);
            }
            else
            {
                for (unsigned int i = 0; i < paletteBanks.Size(); i++)
                    paletteBanks[i].Add(Color16(params.transparent_color));
            }
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

Tileset* Tileset::FromImage(const Image16Bpp& image, int bpp, bool affine)
{
    std::vector<Image16Bpp> images;
    images.push_back(image);
    return new Tileset(images, "", bpp, affine);
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

        std::stringstream oss;
        oss << tile;
        if (!tile_id)
            VerboseLog("Tileset::Match %s %d %d", oss.str().c_str(), tile_id, pal_id);
        return true;
    }

    return false;
}

void Tileset::WriteData(std::ostream& file) const
{
    if (export_shared_data)
    {
        if (bpp == 8)
            palette->WriteData(file);
        else
            paletteBanks.WriteData(file);
    }

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
    if (export_shared_data)
    {
        if (bpp == 8)
            palette->WriteExport(file);
        else
            paletteBanks.WriteExport(file);
    }

    WriteDefine(file, name, "_PALETTE_TYPE", (bpp == 8), 7);
    WriteNewLine(file);

    WriteExtern(file, "const unsigned short", name, "_tiles", Size());
    WriteDefine(file, name, "_TILES", tiles.size());
    WriteDefine(file, name, "_TILES_SIZE", Size() * 2);
    WriteDefine(file, name, "_TILES_LENGTH", Size());
    WriteNewLine(file);
}

void Tileset::Init4bpp(const std::vector<Image16Bpp>& images)
{
    // Tile image into 16 bit tiles
    Tileset tileset16bpp(images, name, 16, affine);
    std::set<ImageTile> imageTiles = tileset16bpp.itiles;

    const Tile& nullTile = Tile::GetNullTile4();
    const ImageTile& nullImageTile = ImageTile::GetNullTile();
    tiles.insert(nullTile);
    tilesExport.push_back(nullTile);
    matcher.insert(std::pair<ImageTile, Tile>(nullImageTile, nullTile));

    // Reduce each tile to 4bpp
    std::vector<Tile> gbaTiles;
    for (const auto& imageTile : imageTiles)
        gbaTiles.emplace_back(imageTile, 4);

    // Ensure image contains < 256 colors
    std::set<Color16> bigPalette;
    for (const auto& tile : gbaTiles)
    {
        const std::vector<Color16>& tile_palette = tile.palette.GetColors();
        bigPalette.insert(tile_palette.begin(), tile_palette.end());
    }

    if (bigPalette.size() > 256 && !params.force)
        FatalLog("Image after reducing tiles to 4 bpp still contains more than 256 distinct colors. Found %d colors. Please fix. Use --force to override.", bigPalette.size());
    else if (bigPalette.size() > 256 && params.force)
        WarnLog("Image after reducing tiles to 4 bpp still contains more than 256 distinct colors. Found %d colors. Potential for image color quality loss.");

    // Greedy approach deal with tiles with largest palettes first.
    std::sort(gbaTiles.begin(), gbaTiles.end(), TilesPaletteSizeComp);

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
            int min_delta = 0x7FFFFFFF;
            for (unsigned int i = 0; i < paletteBanks.Size(); i++)
            {
                PaletteBank& bank = paletteBanks[i];
                int colors_left;
                int delta;
                bank.CanMerge(tile.palette, colors_left, delta);
                if (colors_left >= 0 && delta < min_delta)
                {
                    min_delta = delta;
                    pbank = i;
                }
            }
        }

        // Cry and die for now. Unless you tell me to keep going.
        if (pbank == -1 && !params.force)
            FatalLog("More than 16 distinct palettes found, please use 8bpp mode. Use --force to override.");
        else if (pbank == -1 && params.force)
            WarnLog("More than 16 distinct palettes found. Huge potential for image color quality loss.");

        // Alright...
        if (pbank == -1)
        {
            pbank = paletteBanks.FindBestMatch(tile.palette);
            paletteBanks[pbank].BestMerge(tile.palette);
        }
        else
        {
            // Merge step and assign palette bank
            paletteBanks[pbank].Merge(tile.palette);
        }

        tile.palette_bank = pbank;
        tile.UsePalette(paletteBanks[pbank]);

        // Assign tile id
        const auto& it = tiles.find(tile);
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
        FatalLog("Too many tiles. Found %d tiles. Please make the image simpler.", tiles.size());

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
    Image8BppScene scene(images16, name, palette);
    palette = scene.palette;

    const Tile& nullTile = Tile::GetNullTile8();
    const ImageTile& nullImageTile = ImageTile::GetNullTile();
    tiles.insert(nullTile);
    tilesExport.push_back(nullTile);
    matcher.insert(std::pair<ImageTile, Tile>(nullImageTile, nullTile));

    for (unsigned int k = 0; k < images16.size(); k++)
    {
        const Image8Bpp& image = scene.GetImage(k);
        const Image16Bpp& image16 = images16[k];

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
        }
    }

    // Checks
    int tile_size = TILE_SIZE_BYTES_8BPP;
    int memory_b = tiles.size() * tile_size;
    if (params.force)
    {
        if (!affine && tiles.size() >= 1024)
            WarnLog("Too many tiles. Found %d tiles. Maximum is 1024", tiles.size());
        else if (affine && tiles.size() >= 256)
            WarnLog("Too many tiles found for affine. Found %d tiles. Maximum is 256", tiles.size());
    }
    else
    {
        if (!affine && tiles.size() >= 1024)
            FatalLog("Too many tiles. Found %d tiles. Maximum is 1024. Please make the map/tileset simpler. Use --force to override this.", tiles.size());
        else if (affine && tiles.size() >= 256)
            FatalLog("Too many tiles found for affine. Found %d tiles. Maximum is 256. Please make the map/tileset simpler. Use --force to override this.", tiles.size());
    }

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
