#ifndef TILESET_HPP
#define TILESET_HPP

#include <memory>

#include "exportable.hpp"
#include "palette.hpp"
#include "tile.hpp"

class Image16Bpp;

/** Class represents a set of 8x8 pixel tiles */
class Tileset : public Exportable
{
    public:
        Tileset(const std::vector<Image16Bpp>& images, const std::string& name, int bpp, const std::shared_ptr<Palette>& palette = nullptr);
        static Tileset* FromImage(const Image16Bpp& image, int bpp);
        int Search(const Tile& tile) const;
        int Search(const ImageTile& tile) const;
        // Match Imagetile to Tile (only for bpp = 4)
        bool Match(const ImageTile& tile, int& tile_id, int& pal_id) const;
        unsigned int Size() const {return tiles.size() * ((bpp == 4) ? TILE_SIZE_SHORTS_4BPP : (bpp == 8) ? TILE_SIZE_SHORTS_8BPP : 1);};
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        int bpp;
        // Only one of two will be used bpp = 4 or 8: tiles 16: itiles
        std::set<Tile> tiles;
        std::set<ImageTile> itiles;
        // Bookkeeping matcher used when bpp = 4 or 8
        std::map<ImageTile, Tile> matcher;
        // Tiles sorted by id for export.
        std::vector<Tile> tilesExport;
        // Only one max will be used bpp = 4: paletteBanks 8: palette 16: neither
        std::shared_ptr<Palette> palette;
        PaletteBankManager paletteBanks;
        // Only valid for bpp = 4 and 8
        std::vector<int> offsets;
    private:
        void Init4bpp(const std::vector<Image16Bpp>& images);
        void Init8bpp(const std::vector<Image16Bpp>& images);
        void Init16bpp(const std::vector<Image16Bpp>& images);
        bool export_shared_data;
};

#endif
