#include <memory>
#include <vector>

#include "alltypes.hpp"
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

void DoDSExport(const std::vector<Image32Bpp>& images32, const std::vector<Image32Bpp>& tilesets32, const std::vector<Image32Bpp>& palettes32)
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

    if (params.mode == "TILEMAP")
        DoMode0Export(images);
    else if (params.mode == "BITMAP")
        DoMode3Export(images);
    else if (params.mode == "INDEXED")
        DoMode4Export(images, palette);
    else if (params.mode == "SPRITES")
        DoSpriteExport(images, palette);
    else if (params.mode == "TILES")
        DoTilesetExport(images, palette);
    else if (params.mode == "MAP")
        DoMapExport(images, tilesets);
    else if (params.mode == "PALETTE")
        DoPaletteExport(images);
    else
        FatalLog("No/Invalid mode specified image not exported");
}

