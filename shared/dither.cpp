#include "dither.hpp"

#include <algorithm>
#include <cmath>

#ifndef CLAMP
#define CLAMP(x) (((x) < 0.0) ? 0.0 : (((x) > 31) ? 31 : (x)))
#endif

struct DitherImage
{
    DitherImage(const Image16Bpp& _inImage, Image8Bpp& _outImage, unsigned short _transparent, int _dither, float _ditherlevel) :
        inImage(_inImage), outImage(_outImage), transparent(_transparent), x(0), y(0), dither(_dither), ditherlevel(_ditherlevel) {};
    const Image16Bpp& inImage;
    Image8Bpp& outImage;
    unsigned short transparent;
    unsigned int x, y;
    int dither;
    float ditherlevel;
};

enum
{
    NONE,
    UP,
    LEFT,
    DOWN,
    RIGHT,
};

int Dither(const unsigned short& data, std::shared_ptr<Palette> palette, unsigned short transparent, int dither, float ditherlevel)
{
    static int ex = 0, ey = 0, ez = 0;
    if (data == transparent) return 0;

    Color color(data);
    Color newColor(CLAMP(color.x + ex), CLAMP(color.y + ey), CLAMP(color.z + ez));
    int index = palette->Search(newColor);
    newColor = palette->At(index);

    if (dither)
    {
        ex += color.x - newColor.x;
        ey += color.y - newColor.y;
        ez += color.z - newColor.z;
        ex *= ditherlevel;
        ey *= ditherlevel;
        ez *= ditherlevel;
    }

    return index;
}

static void move(DitherImage& dither, int direction)
{
    const Image16Bpp& image = dither.inImage;
    Image8Bpp& indexedImage = dither.outImage;
    int x = dither.x;
    int y = dither.y;
    int width = indexedImage.width;
    int height = indexedImage.height;
    /* dither the current pixel */
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        int index = Dither(image.pixels[x + y * width], dither.outImage.palette, dither.transparent, dither.dither, dither.ditherlevel);
        indexedImage.pixels[x + y * width] = index;
    }

    /* move to the next pixel */
    switch (direction)
    {
        case LEFT:
            dither.x -= 1;
            break;
        case RIGHT:
            dither.x += 1;
            break;
        case UP:
            dither.y -= 1;
            break;
        case DOWN:
            dither.y += 1;
            break;
    }
}

void Hilbert(DitherImage& dither, int level, int direction)
{
    if (level == 1)
    {
        switch (direction)
        {
            case LEFT:
                move(dither, RIGHT);
                move(dither, DOWN);
                move(dither, LEFT);
                break;
            case RIGHT:
                move(dither, LEFT);
                move(dither, UP);
                move(dither, RIGHT);
                break;
            case UP:
                move(dither, DOWN);
                move(dither, RIGHT);
                move(dither, UP);
                break;
            case DOWN:
                move(dither, UP);
                move(dither, LEFT);
                move(dither, DOWN);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case LEFT:
                Hilbert(dither, level - 1, UP);
                move(dither, RIGHT);
                Hilbert(dither, level - 1, LEFT);
                move(dither, DOWN);
                Hilbert(dither, level - 1, LEFT);
                move(dither, LEFT);
                Hilbert(dither, level - 1, DOWN);
                break;
            case RIGHT:
                Hilbert(dither, level - 1, DOWN);
                move(dither, LEFT);
                Hilbert(dither, level - 1, RIGHT);
                move(dither, UP);
                Hilbert(dither, level - 1, RIGHT);
                move(dither, RIGHT);
                Hilbert(dither, level - 1, UP);
                break;
            case UP:
                Hilbert(dither, level - 1, LEFT);
                move(dither, DOWN);
                Hilbert(dither, level - 1, UP);
                move(dither, RIGHT);
                Hilbert(dither, level - 1, UP);
                move(dither, UP);
                Hilbert(dither, level - 1, RIGHT);
                break;
            case DOWN:
                Hilbert(dither, level - 1, RIGHT);
                move(dither, UP);
                Hilbert(dither, level - 1, DOWN);
                move(dither, LEFT);
                Hilbert(dither, level - 1, DOWN);
                move(dither, DOWN);
                Hilbert(dither, level - 1, LEFT);
                break;
        }
    }
}

void RiemersmaDither(const Image16Bpp& inImage, Image8Bpp& outImage, unsigned short transparent, int dither, float ditherlevel)
{
    DitherImage dimage(inImage, outImage, transparent, dither, ditherlevel);
    int size = ceil(log2(std::max(inImage.width, inImage.height)));
    if (size > 0) Hilbert(dimage, size, UP);
    move(dimage, NONE);
}
