#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <memory>
#include <vector>

#include "image.hpp"
#include "palette.hpp"
#include "scene.hpp"
#include "tile.hpp"

class Image16Bpp;

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
        int bpp;

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
        SpriteSheet(const std::vector<Sprite*>& sprites, const std::string& name, int bpp);
        void Compile();
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::vector<unsigned char> data;
        unsigned int width, height;
        std::string name;
        int bpp;
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
        SpriteScene(const std::vector<Image16Bpp>& images, const std::string& name, bool is2d, std::shared_ptr<Palette>& global_palette);
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
    private:
        void Init4bpp(const std::vector<Image16Bpp>& images);
        void Init8bpp(const std::vector<Image16Bpp>& images);
};

#endif
