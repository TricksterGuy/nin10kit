#include "color.hpp"
#include "cpercep.hpp"
#include "logger.hpp"

#define LOWA (-86.181F)
#define LOWB (-107.858F)
#define HIGHA (98.237F)
#define HIGHB (94.480F)
#define LRAT (2.55F)
#define ARAT (255.0F / (HIGHA - LOWA))
#define BRAT (255.0F / (HIGHB - LOWB))

#ifndef CLAMP
#define CLAMP(x, l, u) (((x) < l) ? l : (((x) > u) ? u : (x)))
#endif

void rgb_to_lin(const unsigned char r, const unsigned char g, const unsigned char b, int *hr, int *hg, int *hb)
{
    double lab[3];

    cpercep_rgb_to_space(r / 255.0, g / 255.0, b / 255.0, lab, lab + 1, lab + 2);

    int ro = (int)(lab[0] * LRAT);
    int go = (int)((lab[1] - LOWA) * ARAT);
    int bo = (int)((lab[2] - LOWB) * BRAT);

    *hr = CLAMP(ro, 0, 255);
    *hg = CLAMP(go, 0, 255);
    *hb = CLAMP(bo, 0, 255);
}

void lin_to_rgb(const double hr, const double hg, const double hb, unsigned char *r, unsigned char *g, unsigned char *b)
{
    double rgb[3];

    double ir = hr / LRAT;
    double ig = hg / ARAT + LOWA;
    double ib = hb / BRAT + LOWB;

    cpercep_space_to_rgb(ir, ig, ib, rgb, rgb + 1, rgb + 2);

    *r = (int)CLAMP(rgb[0] * 255, 0.0F, 255.0F);
    *g = (int)CLAMP(rgb[1] * 255, 0.0F, 255.0F);
    *b = (int)CLAMP(rgb[2] * 255, 0.0F, 255.0F);
}

static inline bool GenericComponentCompare(int a1, int b1, int c1, int a2, int b2, int c2)
{
    if (a1 != a2)
        return a1 < a2;
    if (b1 != b2)
        return b1 < b2;
    return c1 < c2;
}

static inline bool GenericComponentEquals(int a1, int b1, int c1, int a2, int b2, int c2)
{
    return a1 == a2 && b1 == b2 && c1 == c2;
}

static inline unsigned long GenericComponentDistance(int a1, int b1, int c1, int a2, int b2, int c2)
{
    const unsigned long ad = a1 - a2;
    const unsigned long bd = b1 - b2;
    const unsigned long cd = c1 - c2;
    return (ad * ad + bd * bd + cd * cd);
}

Color::Color(const ColorLAB& color)
{
    lin_to_rgb(color.l, color.a, color.b, &r, &g, &b);
}

bool Color::operator<(const Color& color) const
{
    return GenericComponentCompare(r, g, b, color.r, color.g, color.b);
}

bool Color::operator==(const Color& color) const
{
    return GenericComponentEquals(r, g, b, color.r, color.g, color.b);
}

Color16::Color16(const ColorLAB& color)
{
    lin_to_rgb(color.l, color.a, color.b, &r, &g, &b);
    r >>= 3;
    g >>= 3;
    b >>= 3;
}

bool Color16::operator<(const Color16& color) const
{
    return GenericComponentCompare(r, g, b, color.r, color.g, color.b);
}

bool Color16::operator==(const Color16& color) const
{
    return GenericComponentEquals(r, g, b, color.r, color.g, color.b);
}

ColorLAB::ColorLAB(const Color& color)
{
    rgb_to_lin(color.r, color.g, color.b, &l, &a, &b);
}

ColorLAB::ColorLAB(const Color16& color)
{
    rgb_to_lin(color.r << 3, color.g << 3, color.b << 3, &l, &a, &b);
}

bool ColorLAB::operator<(const ColorLAB& color) const
{
    return GenericComponentCompare(l, a, b, color.l, color.a, color.b);
}

bool ColorLAB::operator==(const ColorLAB& color) const
{
    return GenericComponentEquals(l, a, b, color.l, color.a, color.b);
}

unsigned long ColorLAB::Distance(const ColorLAB& color) const
{
    return GenericComponentDistance(l, a, b, color.l, color.a, color.b);
}
