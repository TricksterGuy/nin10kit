#include "color.hpp"
#include "cpercep.hpp"
#include "logger.hpp"

bool Color::operator==(const Color& other) const
{
    return r == other.r && g == other.g && b == other.b;
}

bool Color::operator<(const Color& right) const
{
    bool less;
    less = r < right.r;
    if (r == right.r) less = g < right.g;
    if (r == right.r && g == right.g) less = b < right.b;
    return less;
}

Color Color::FromBGR15(unsigned short color_data)
{
    return Color((color_data & 0x1f) << 3, ((color_data >> 5) & 0x1f) << 3, ((color_data >> 10) & 0x1f) << 3);
}

void Color::SetBGR15(unsigned short color_data)
{
    r = (color_data & 0x1f) << 3;
    g = ((color_data >> 5) & 0x1f) << 3;
    b = ((color_data >> 10) & 0x1f) << 3;
}

void Color::SetBGR15(const Color& other)
{
    r = other.r & 0xf8;
    g = other.g & 0xf8;
    b = other.b & 0xf8;
}

unsigned short Color::GetBGR15() const
{
    return ((r >> 3) & 0x1f) | (((g >> 3) & 0x1f) << 5) | (((b >> 3) & 0x1f) << 10);
}

void Color::GetBGR15(unsigned char& x, unsigned char& y, unsigned char& z) const
{
    x = (r >> 3) & 0x1f;
    y = (g >> 3) & 0x1f;
    z = (b >> 3) & 0x1f;
}

double Color::Distance(const Color& other) const
{
    unsigned char ro, go, bo;
    double l, a, lb, ol, oa, ob;

    ro = other.r;
    go = other.g;
    bo = other.b;

    cpercep_rgb_to_space(r, g, b, &l, &a, &lb);
    cpercep_rgb_to_space(ro, go, bo, &ol, &oa, &ob);

    return cpercep_distance_space(l, a, lb, ol, oa, ob);
}
