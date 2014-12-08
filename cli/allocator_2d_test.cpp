#include <vector>
#include <algorithm>
#include <map>
#include <list>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

int valid_sizes[16] =
{
    0x0, 0x1, 0x2, -1,
    0x4, 0x5, 0x6, -1,
    0x8, 0x9, 0xA, 0xB,
    -1, -1, 0xE, 0xF,
};

class Sprite
{
    public:
        Sprite() : width(0), height(0) {};
        Sprite(unsigned int w, unsigned int h) : width(w), height(h) {};
        unsigned int Size() const {return width * height;};
        std::string name;
        unsigned int width;
        unsigned int height;
        int offset;
};

class BlockSize
{
    public:
        BlockSize() : width(0), height(0) {};
        BlockSize(unsigned int _width, unsigned int _height) : width(_width), height(_height) {};
        bool operator==(const BlockSize& rhs) const;
        bool operator<(const BlockSize& rhs) const;
        unsigned int Size() const {return width * height;};
        bool isBiggestSize() const {return width == 8 and height == 8;};
        static std::vector<BlockSize> BiggerSizes(const BlockSize& b);
        unsigned int width, height;

};

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

class SpriteSheet
{
    public:
        SpriteSheet(const std::vector<Sprite>& sprites, unsigned int width, unsigned int height);
        void Compile();
        std::map<BlockSize, std::list<Block>> freeBlocks;
        std::vector<Sprite> sprites;
        std::list<Block> placedBlocks;
        unsigned int width, height;
        std::vector<int> data;
    private:
        void PlaceSprites();
        bool AssignBlockIfAvailable(BlockSize& size, Sprite& sprite, unsigned int i);
        bool HasAvailableBlock(const BlockSize& size);
        void SliceBlock(const BlockSize& size, const std::list<BlockSize>& slice);
};


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
    {
        printf("%d %d => %d %d\n", size.width, size.height, to_this_size.width, to_this_size.height);
        throw "Error Block::Split";
    }

    return Block();
}

SpriteSheet::SpriteSheet(const std::vector<Sprite>& _sprites, unsigned int _width, unsigned int _height) : sprites(_sprites), width(_width), height(_height), data(width * height)
{
}

bool SpriteCompare(const Sprite& lhs, const Sprite& rhs)
{
    if (lhs.Size() != rhs.Size())
        return lhs.Size() > rhs.Size();
    else
        // Special case 2x2 should be of lesser priority than 4x1/1x4
        return lhs.width + lhs.height > rhs.width + rhs.height;
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
                data[y * width + x] = block.sprite_id;
            }
        }
    }
}

void SpriteSheet::PlaceSprites()
{
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
        Sprite& sprite = sprites[i];
        printf("request %d %d\n", sprites[i].width, sprites[i].height);
        BlockSize size(sprite.width, sprite.height);
        std::list<BlockSize> slice;

        // Mother may I have block of this size?
        if (AssignBlockIfAvailable(size, sprite, i))
            goto allocated;

        if (size.isBiggestSize())
        {
            std::stringstream oss;
            oss << "[FATAL] Out of sprite memory could not allocate sprite size (" << sprite.width << "," << sprite.height << ")";
            throw oss.str();
        }

        slice.push_front(size);
        while (!HasAvailableBlock(size))
        {
            std::vector<BlockSize> sizes = BlockSize::BiggerSizes(size);
            if (sizes.empty())
            {
                std::stringstream oss;
                oss << "[FATAL] Out of sprite memory could not allocate sprite size (" << sprite.width << "," << sprite.height << ")";
                throw oss.str();
            }

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
        {
            std::stringstream oss;
            oss << "[FATAL] Out of sprite memory could not allocate sprite size (" << sprite.width << "," << sprite.height << ")";
            throw oss.str();
        }

        printf("BEFORE SPLIT\n");
        for (const auto& size_list : freeBlocks)
        {
            printf("(%d %d) => ", size_list.first.width, size_list.first.height);
            for (const auto& block : size_list.second)
            {
                printf("(%d %d %d %d), ", block.x, block.y, block.size.width, block.size.height);
            }
            printf("\n");
        }
        SliceBlock(size, slice);
        printf("AFTER SPLIT\n");
        for (const auto& size_list : freeBlocks)
        {
            printf("(%d %d) => ", size_list.first.width, size_list.first.height);
            for (const auto& block : size_list.second)
            {
                printf("(%d %d %d %d), ", block.x, block.y, block.size.width, block.size.height);
            }
            printf("\n");
        }

        printf("FIN\n");
        size = BlockSize(sprite.width, sprite.height);
        // Mother may I have block of this size?
        if (AssignBlockIfAvailable(size, sprite, i))
            goto allocated;
        else
        {
            std::stringstream oss;
            oss << "[FATAL] Out of sprite memory could not allocate sprite size (" << sprite.width << "," << sprite.height << ")";
            throw oss.str();
        }

        allocated:
        for (const auto& size_list : freeBlocks)
        {
            printf("(%d %d) => ", size_list.first.width, size_list.first.height);
            for (const auto& block : size_list.second)
            {
                printf("(%d %d %d %d), ", block.x, block.y, block.size.width, block.size.height);
            }
            printf("\n");
        }
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
        sprite.offset = allocd.y * width + allocd.x;
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

int main(int argc, char** argv)
{
    srand(time(NULL));
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int total_size = 0;
    int max_size = width * height;

    std::vector<Sprite> sprites;
    while (total_size < max_size)
    {
        int i = rand() & 0xF;

        int size = valid_sizes[i];
        if (size == -1) continue;
        int sw = 1 << (size >> 2);
        int sh = 1 << (size & 3);

        if (total_size + sw * sh > max_size) continue;

        sprites.push_back(Sprite(sw, sh));
        total_size += sw * sh;
    }

    for (const auto& sprite : sprites)
    {
        printf("%d %d\n", sprite.width, sprite.height);
    }

    try
    {
        SpriteSheet sheet(sprites, width, height);
        sheet.Compile();

        for (unsigned int y = 0; y < sheet.height; y++)
        {
            for (unsigned int x = 0; x < sheet.width; x++)
            {
                printf("%02d ", sheet.data[y * sheet.width + x]);
            }
            printf("\n");
        }
    }
    catch (const std::exception& ex)
    {
        printf("Image to GBA (sprites) failed! Reason: %s\n", ex.what());
        exit(EXIT_FAILURE);
    }
    catch (const std::string& ex)
    {
        printf("Image to GBA (sprites) failed! Reason: %s\n", ex.c_str());
        exit(EXIT_FAILURE);
    }
    catch (const char* ex)
    {
        printf("Image to GBA (sprites) failed! Reason: %s\n", ex);
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        printf("Image to GBA (sprites) failed!");
        exit(EXIT_FAILURE);
    }
}
