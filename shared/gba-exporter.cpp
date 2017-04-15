#include <memory>
#include <vector>

#include "alltypes.hpp"
#include "export_params.hpp"
#include "fileutils.hpp"
#include "logger.hpp"
#include "shared.hpp"

void DoMode0Export(const std::vector<Image16Bpp>& images);
void DoMode3Export(const std::vector<Image16Bpp>& images);
void DoMode4Export(const std::vector<Image16Bpp>& images, const std::shared_ptr<Palette>& palette);
void DoPaletteExport(const std::vector<Image16Bpp>& images);
void DoTilesetExport(const std::vector<Image16Bpp>& images, const std::shared_ptr<Palette>& palette);
void DoMapExport(const std::vector<Image16Bpp>& images, const std::vector<Image16Bpp>& tilesets);
void DoSpriteExport(const std::vector<Image16Bpp>& images, const std::shared_ptr<Palette>& palette);

void DoGBAExport(const std::vector<Image32Bpp>& images32, const std::vector<Image32Bpp>& tilesets32, const std::vector<Image32Bpp>& palettes32)
{
    std::vector<Image16Bpp> images;
    for (const auto& image : images32)
        images.push_back(Image16Bpp(image));

    std::vector<Image16Bpp> tilesets;
    for (const auto& image : tilesets32)
        tilesets.push_back(Image16Bpp(image));

    std::shared_ptr<Palette> palette;
    if (!palettes32.empty())
    {
        std::vector<Image16Bpp> palettes;
        for (const auto& image : palettes32)
            palettes.push_back(Image16Bpp(image));
        Image8BppScene scene(palettes, params.symbol_base_name);
        palette = scene.palette;
    }

    if (params.mode == "0" || params.mode == "TILEMAP")
        DoMode0Export(images);
    else if (params.mode == "3" || params.mode == "BITMAP")
        DoMode3Export(images);
    else if (params.mode == "4" || params.mode == "INDEXED")
        DoMode4Export(images, palette);
    else if (params.mode == "SPRITES")
        DoSpriteExport(images, palette);
    else if (params.mode == "TILES")
        DoTilesetExport(images, palette);
    else if (params.mode == "MAP")
        DoMapExport(images, tilesets);
    else if (params.mode == "PALETTE")
        DoPaletteExport(images);
}

void DoMode0Export(const std::vector<Image16Bpp>& images)
{
    // If split then form several maps
    // If !split then start a scene
    // Add appropriate object to header/implementation
    if (params.split)
    {
        for (const auto& image : images)
        {
            std::shared_ptr<Exportable> map_ptr(new Map(image, params.bpp));
            header.Add(map_ptr);
            implementation.Add(map_ptr);
        }
    }
    else
    {
        std::shared_ptr<Exportable> scene(new MapScene(images, params.symbol_base_name, params.bpp));
        header.Add(scene);
        implementation.Add(scene);
    }
}

void DoMode3Export(const std::vector<Image16Bpp>& images)
{
    for (const auto& image : images)
    {
        std::shared_ptr<Exportable> image_ptr(new Image16Bpp(image));
        header.Add(image_ptr);
        implementation.Add(image_ptr);
    }
}

void DoMode4Export(const std::vector<Image16Bpp>& images, const std::shared_ptr<Palette>& palette)
{
    // If split then get vector of 8 bit images
    // If !split then cause a scene.
    // Add appropriate object to header/implementation.
    if (params.split)
    {
        for (const auto& image : images)
        {
            std::shared_ptr<Exportable> image_ptr(new Image8Bpp(image, palette));
            header.Add(image_ptr);
            implementation.Add(image_ptr);
        }
    }
    else
    {
        std::shared_ptr<Exportable> scene(new Image8BppScene(images, params.symbol_base_name, palette));
        header.Add(scene);
        implementation.Add(scene);
    }
}

void DoPaletteExport(const std::vector<Image16Bpp>& images)
{
    // Dummy scene
    Image8BppScene scene(images, params.symbol_base_name);

    auto exportable = std::static_pointer_cast<Exportable>(scene.palette);
    header.Add(exportable);
    implementation.Add(exportable);
}

void DoSpriteExport(const std::vector<Image16Bpp>& images, const std::shared_ptr<Palette>& palette)
{
    // Do the work of sprite conversion.
    // Form the sprite scene and then add it to header and implementation
    SpriteScene* scene(new SpriteScene(images, params.symbol_base_name, params.export_2d, params.bpp, palette));
    std::shared_ptr<Exportable> exportable(scene);

    // Build the sprite scene and place all sprites. (If applicable)
    scene->Build();

    header.Add(exportable);
    implementation.Add(exportable);
}

void DoTilesetExport(const std::vector<Image16Bpp>& images, const std::shared_ptr<Palette>& palette)
{
    // Form the tileset and then add it to header and implementation
    std::shared_ptr<Exportable> tileset(new Tileset(images, params.symbol_base_name, params.bpp, palette));

    header.Add(tileset);
    implementation.Add(tileset);
}

void DoMapExport(const std::vector<Image16Bpp>& images, const std::vector<Image16Bpp>& tilesets)
{
    if (tilesets.empty())
        FatalLog("Map export specified however -tileset not given");

    // Form the tileset from the images given this is a dummy
    std::shared_ptr<Tileset> tileset(new Tileset(tilesets, "", params.bpp));

    for (const auto& image : images)
    {
        std::shared_ptr<Exportable> map_ptr(new Map(image, tileset));
        header.Add(map_ptr);
        implementation.Add(map_ptr);
    }
}
