#ifndef COLOR_HPP
#define COLOR_HPP

class ColorLAB;

/** Normal everyday 8 bpp color with alpha */
class Color
{
    public:
        Color() : a(0), r(0), g(0), b(0) {}
        Color(unsigned int data) :  a(data >> 24 & 0xFF), r(data >> 16 & 0xFF), g(data >> 8 & 0xFF), b(data & 0xFF) {}
        Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 0) : a(_a), r(_r), g(_g), b(_b) {}
        Color(const ColorLAB& color);
        bool operator<(const Color& color) const;
        bool operator==(const Color& color) const;
        bool operator!=(const Color& color) const {return !(*this == color);}
        /** Color components range 0-255 */
        unsigned char a, r, g, b;
};

/** 16 Bit Color ABGR with A=1 R,G,B=5 */
class Color16
{
    public:
        Color16() : a(0), r(0), g(0), b(0) {}
        Color16(unsigned char _r, unsigned char _g, unsigned char _b, bool _a = 0) : a(_a), r(_r), g(_g), b(_b) {}
        Color16(const Color& color) : a(color.a > 127), r(color.r >> 3), g(color.g >> 3), b(color.b >> 3) {}
        Color16(const ColorLAB& color);
        bool operator<(const Color16& color) const;
        bool operator==(const Color16& color) const;
        bool operator!=(const Color16& color) const {return !(*this == color);}
        unsigned short ToGBAShort() const {return r | (g << 5) | (b << 10);}
        unsigned short ToDSShort() const {return r | (g << 5) | (b << 10) | (a << 15);}
        Color ToColor() const {return Color(r << 3, g << 3, b << 3, a * 255);}
        /** Color components range 0-31 */
        bool a;
        unsigned char r, g, b;
};

/** Color in the LAB colorspace */
class ColorLAB
{
    public:
        ColorLAB() : l(0), a(0), b(0) {}
        ColorLAB(unsigned char _l, unsigned char _a, unsigned char _b) :l(_l), a(_a), b(_b) {}
        ColorLAB(const Color& color);
        ColorLAB(const Color16& color);
        bool operator<(const ColorLAB& color) const;
        bool operator==(const ColorLAB& color) const;
        bool operator!=(const ColorLAB& color) const {return !(*this == color);}
        unsigned long Distance(const ColorLAB& color) const;
        int l, a, b;
};

#endif
