#include "map.hpp"

#include "logger.hpp"
#include "fileutils.hpp"
#include "image16.hpp"
#include "tileset.hpp"

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

    WriteBeginArray(file, "const unsigned short", export_name, "", num_blocks * 32 * 32);
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

    WriteExtern(file, "const unsigned short", export_name, "", data.size());
    if (!animated)
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
