#ifndef REDUCTION_HELPER_HPP
#define REDUCTION_HELPER_HPP

#include <iterator>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <Magick++.h>

#define TOTAL_TILE_MEMORY_BYTES 65536
#define TILE_SIZE 64
#define PALETTE_SIZE 16
#define TILE_SIZE_BYTES_8BPP 64
#define TILE_SIZE_BYTES_4BPP 32
#define TILE_SIZE_SHORTS_8BPP 32
#define TILE_SIZE_SHORTS_4BPP 16
#define SIZE_CBB_BYTES (8192 * 2)
#define SIZE_SBB_BYTES (1024 * 2)
#define SIZE_SBB_SHORTS 1024

#include "color.hpp"
#include "exportable.hpp"
#include "logger.hpp"

/** Represents a single frame of an image */
class Image : public Exportable
{
    public:
        Image(unsigned int _width, unsigned int _height, const std::string& _name = "", const std::string& _filename = "", unsigned int _frame = 0, bool _animated = false) :
            Exportable(_name), width(_width), height(_height), filename(_filename), frame(_frame), animated(_animated)
        {
            if (animated)
            {
                std::stringstream oss;
                oss << name << frame;
                export_name = oss.str();
            }
            else
                export_name = name;
        }
        Image(const Image& image) : Exportable(image), width(image.width), height(image.height), filename(image.filename), frame(image.frame),
            animated(image.animated), export_name(image.export_name) {}
        virtual ~Image() {}
        /** If Image is actually part of an animated Image export information common to all of these images */
        virtual void WriteCommonExport(std::ostream& file) const = 0;
        /** Gets type of image array for generating an array of images for animation.  */
        virtual std::string GetImageType() const {return "const unsigned short*";}
        /** Gets the symbol base name for this image */
        virtual std::string GetExportName() const {return export_name;}
        /** Width of image in either pixels or tiles */
        unsigned int width;
        /** Height of image in either pixels or tiles */
        unsigned int height;
        /** Filename of image */
        std::string filename;
        /** Frame of image if this image came from an animated image */
        unsigned int frame;
        /** Is this image part of an animated image */
        bool animated;
    protected:
        /** Symbol base name */
        std::string export_name;
};

/** Represents a set of images who all share some resource (palette, tileset) */
class Scene : public Exportable
{
    public:
        Scene(const std::string& _name) : Exportable(_name) {}
        virtual ~Scene() {}
        std::vector<std::unique_ptr<Image>>& GetImages() {return images;}
        virtual void WriteData(std::ostream& file) const
        {
            for (const auto& image : images)
                image->WriteData(file);
        }
        virtual void WriteExport(std::ostream& file) const
        {
            for (const auto& image : images)
                image->WriteExport(file);
        }
    protected:
        /** Images contained in this scene */
        std::vector<std::unique_ptr<Image>> images;
};

/** Represents a 32 (or 24) Bits per pixel image
  * ImageMagick Images are converted to this class first
  * Used for 3ds exports.
  */
class Image32Bpp : public Image
{
    public:
        Image32Bpp(const Magick::Image& image, const std::string& name, const std::string& filename, unsigned int frame, bool animated);
        void WriteData(std::ostream& file) const;
        void WriteCommonExport(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        virtual std::string GetImageType() const {return "const unsigned char*";}
        std::vector<Color> pixels;
        bool has_alpha;
};

/** 16 Bit Image BGR
  * Used for GBA mode 3.
  */
class Image16Bpp : public Image
{
    public:
        Image16Bpp(const Image32Bpp& image);
        void WriteData(std::ostream& file) const;
        void WriteCommonExport(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        /** Gets array of colors excluding transparent color */
        void GetColors(std::vector<Color>& colors) const;
        std::vector<Color> pixels;
};

/** Base class for palettes/palette banks.  Represents a set of colors. */
class ColorArray
{
    public:
        ColorArray(const std::vector<Color>& _colors = std::vector<Color>()) : colors(_colors), colorSet(colors.begin(), colors.end()) {}
        /** Sets array to contain colors passed in */
        void Set(const std::vector<Color>& _colors);
        /** Gets color at palette index */
        const Color& At(int index) const {return colors[index];}
        /** Search palette for color passed in returns the closest palette index that matches the color */
        int Search(const Color& color) const;
        /** Are all colors contained in the palette? */
        bool Contains(const ColorArray& palette) const;
        /** Adds a color to the palette */
        void Add(const Color& c);
        /** Gets size of this palette */
        unsigned int Size() const {return colors.size();};
        /** Gets colors in palette */
        const std::vector<Color> GetColors() const {return colors;}
    protected:
        /** Colors contained in palette with set to prevent duplicates */
        std::vector<Color> colors;
        std::set<Color> colorSet;
        /** Cache to speed up repeatedly hit colors */
        mutable std::map<Color, int> colorIndexCache;
};

/** Palette class, just an exportable color array */
class Palette : public ColorArray, public Exportable
{
    public:
        Palette(const std::string& name = "") : Exportable(name) {} // Used by Tile constructor
        Palette(const std::vector<Color>& _colors, const std::string& _name);
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
};

/** 8 Bit Image with palette
  * Used for GBA mode 4
  */
class Image8Bpp : public Image
{
    public:
        Image8Bpp(const Image16Bpp& image);
        Image8Bpp(const Image16Bpp& image, std::shared_ptr<Palette>& global_palette);
        void WriteData(std::ostream& file) const;
        void WriteCommonExport(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::vector<unsigned char> pixels;
        /** Palette this class uses */
        std::shared_ptr<Palette> palette;
    private:
        /** If true also export palette. */
        bool export_shared_info;
};

/** Represents a set of 8 bit images that share a palette
  * Used for GBA mode 4 in batch mode
  */
class Image8BppScene : public Scene
{
    public:
        Image8BppScene(const std::vector<Image16Bpp>& images, const std::string& name);
        const Image8Bpp& GetImage(int index) const;
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::shared_ptr<Palette> palette;
};

/** Represents a palette bank for 4 bpp exports. */
class PaletteBank : public ColorArray
{
    public:
        PaletteBank(int _id = -1) : id(_id) {};
        PaletteBank(const std::vector<Color>& _colors, int id);
        int CanMerge(const ColorArray& palette) const;
        void Merge(const ColorArray& palette);
        int id;
};

/** Represents 16 palette banks for 4 bpp exports. */
class PaletteBankManager : public Exportable
{
    public:
        PaletteBankManager(const std::string& _name);
        PaletteBankManager(const std::string& _name, const std::vector<PaletteBank>& paletteBanks);
        PaletteBank& operator[](int i) {return banks[i];}
        const PaletteBank& operator[](int i) const {return banks[i];}
        unsigned int Size() const {return banks.size();}
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::vector<PaletteBank> banks;
};

/** This class represents a one to one mapping between an Image and Tile. */
class ImageTile
{
    public:
        ImageTile(const Image16Bpp& image, int tilex, int tiley, int border = 0);
        ~ImageTile() {};
        bool IsEqual(const ImageTile& other) const;
        bool IsSameAs(const ImageTile& other) const;
        bool operator<(const ImageTile& other) const;
        bool operator==(const ImageTile& other) const;
        static const ImageTile& GetNullTile()
        {
            static ImageTile nullTile;
            return nullTile;
        }
        int id;
        std::vector<Color> pixels;
    private:
        ImageTile() : id(0), pixels(TILE_SIZE) {}
};

/* */
class Tile
{
    public:
        Tile(const Image16Bpp& image, int tilex, int tiley, int border = 0, int bpp = 8);
        Tile(const Image8Bpp& image, int tilex, int tiley, int border = 0, int bpp = 8);
        Tile(const Image16Bpp& image, std::shared_ptr<Palette>& global_palette, int tilex, int tiley, int border = 0);
        Tile(std::shared_ptr<ImageTile>& imageTile, int bpp);
        ~Tile() {};
        bool IsEqual(const Tile& other) const;
        bool IsSameAs(const Tile& other) const;
        bool operator<(const Tile& other) const;
        bool operator==(const Tile& other) const;
        /* Set to use palette bank only for 4bpp tiles */
        void UsePalette(const PaletteBank& bank);
        static const Tile& GetNullTile8()
        {
            static Tile nullTile(8);
            return nullTile;
        }
        static const Tile& GetNullTile4()
        {
            static Tile nullTile(4);
            return nullTile;
        }
        int id;
        std::vector<unsigned char> pixels;
        int bpp;
        int palette_bank;
        Palette palette;
        std::shared_ptr<ImageTile> sourceTile;

    friend std::ostream& operator<<(std::ostream& file, const Tile& tile);
    private:
        Tile(int _bpp) : id(0), pixels(TILE_SIZE), bpp(_bpp), palette_bank(0) {}
};

bool TilesPaletteSizeComp(const Tile& i, const Tile& j);

/** Class represents a set of 8x8 pixel tiles */
class Tileset : public Exportable
{
    public:
        Tileset(const std::vector<Image16Bpp>& images, const std::string& name, int bpp);
        Tileset(const Image16Bpp& image, int bpp);
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
};

/** Class representing a map can be 4 or 8 bpp
  * Maps can only be 256x256 512x256 256x512 or 512x512
  */
class Map : public Image
{
    public:
        Map(const Image16Bpp& image, int bpp);
        Map(const Image16Bpp& image, std::shared_ptr<Tileset>& global_tileset);
        void WriteData(std::ostream& file) const;
        void WriteCommonExport(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::vector<unsigned short> data;
        std::shared_ptr<Tileset> tileset;
    private:
        void Init4bpp(const Image16Bpp& image);
        void Init8bpp(const Image16Bpp& image);
        bool export_shared_info;
};

/** Class representing a set of maps which use the same shared Tileset */
class MapScene : public Scene
{
    public:
        MapScene(const std::vector<Image16Bpp>& images, const std::string& name, int bpp);
        MapScene(const std::vector<Image16Bpp>& images, const std::string& name, std::shared_ptr<Tileset>& tileset);
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::shared_ptr<Tileset> tileset;
};

/** A GBA Sprite image
  * Sprites are composed of a set of Tiles in 4 or 8bpp mode
  * Sprites can only be sized (8x8 8x16 8x32 16x8 32x8 16x16 16x32 32x16 32x32 32x64 64x32 64x64
  */
class Sprite : public Image
{
    public:
        Sprite(const Image16Bpp& image, std::shared_ptr<Palette>& global_palette);
        Sprite(const Image16Bpp& image, int bpp);
        unsigned int Size() const {return width * height;};
        void UsePalette(const PaletteBank& bank);
        // Not implemented since this data is written as part of SpriteScene/Sheet
        void WriteData(std::ostream& file) const {}
        void WriteCommonExport(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        virtual std::string GetImageType() const {return "const unsigned short";}
        virtual std::string GetExportName() const;
        void WriteTile(unsigned char* arr, int x, int y) const;
        std::vector<Tile> data;
        std::shared_ptr<Palette> palette;
        int palette_bank;
        int size;
        int shape;
        int offset;

    friend std::ostream& operator<<(std::ostream& file, const Sprite& sprite);
};

/** Represents a block size allocation from spritesheet. */
class BlockSize
{
    public:
        BlockSize() : width(0), height(0) {};
        BlockSize(unsigned int _width, unsigned int _height) : width(_width), height(_height) {};
        bool operator==(const BlockSize& rhs) const;
        bool operator<(const BlockSize& rhs) const;
        unsigned int Size() const {return width * height;};
        bool IsBiggestSize() const {return width == 8 and height == 8;};
        static std::vector<BlockSize> BiggerSizes(const BlockSize& b);
        unsigned int width, height;
};

/** Represents a block allocated from spritesheet */
class Block
{
    public:
        Block() : x(0), y(0), sprite_id(-1) {};
        Block(int width, int height) : size(width, height), x(0), y(0), sprite_id(-1) {};
        Block(const BlockSize& _size) : size(_size), x(0), y(0), sprite_id(-1) {};
        Block(int _x, int _y, int width, int height) : size(width, height), x(_x), y(_y), sprite_id(-1) {};
        Block(int _x, int _y, const BlockSize& _size) : size(_size), x(_x), y(_y), sprite_id(-1) {};
        Block HSplit();
        Block VSplit();
        Block Split(const BlockSize& to_this_size);
        BlockSize size;
        int x;
        int y;
        int sprite_id;
};

/** Spritesheet helper class for arranging 2D sprite sheets from a set of sprites */
class SpriteSheet
{
    public:
        SpriteSheet(const std::vector<Sprite*>& sprites, const std::string& name, int bpp, bool spriteSheetGiven);
        void Compile();
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::vector<unsigned char> data;
        unsigned int width, height;
        std::string name;
        int bpp;
        bool spriteSheetGiven;
    private:
        void PlaceSprites();
        bool AssignBlockIfAvailable(BlockSize& size, Sprite& sprite, unsigned int i);
        bool HasAvailableBlock(const BlockSize& size);
        void SliceBlock(const BlockSize& size, const std::list<BlockSize>& slice);
        std::map<BlockSize, std::list<Block>> freeBlocks;
        std::list<Block> placedBlocks;
        // Not owned by this object, but SpriteScene.
        std::vector<Sprite*> sprites;
};

/** Represents a set of sprites who share the same palette, and Characterblock space */
class SpriteScene : public Scene
{
    public:
        SpriteScene(const std::vector<Image16Bpp>& images, const std::string& name, bool is2d, int bpp);
        SpriteScene(const std::vector<Image16Bpp>& images, const std::string& name, bool is2d, std::shared_ptr<Palette>& tileset);
        SpriteScene(const std::vector<Image16Bpp>& images, const std::string& name, bool is2d, const std::vector<PaletteBank>& paletteBanks);
        void Build();
        const Sprite& GetSprite(int index) const;
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        unsigned int Size() const;
        int bpp;
        // Only one max will be used bpp = 4: paletteBanks 8: palette
        std::shared_ptr<Palette> palette;
        PaletteBankManager paletteBanks;
        // Used if is2d is true
        std::unique_ptr<SpriteSheet> spriteSheet;
        bool is2d;
        // Special for use spritesheet mode
        bool spriteSheetGiven;
    private:
        void Init4bpp(const std::vector<Image16Bpp>& images);
        void Init8bpp(const std::vector<Image16Bpp>& images);
};

void GetPalette(const std::vector<Color>& image, unsigned int num_colors, const Color& transparent, unsigned int offset, Palette& palette);
void QuantizeImage(const std::vector<Color>& image, unsigned int num_colors, const Color& transparent, unsigned int offset, Palette& palette,
                   std::vector<unsigned char>& indexed);
void DitherAndReduceImage(const Image16Bpp& image, const Color& transparent, bool dither, double dither_level, unsigned int offset, Image8Bpp& indexedImage);

#endif
