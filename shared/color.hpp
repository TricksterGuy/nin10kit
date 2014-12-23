#ifndef COLOR_HPP
#define COLOR_HPP

///TODO rewrite this entire class.
class Color
{
    public:
        Color() {}
        Color(unsigned int data) : r(data >> 16 & 0xFF), g(data >> 8 & 0xFF), b(data & 0xFF), a(data >> 24 & 0xFF) {}
        Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 0) : r(_r), g(_g), b(_b), a(_a) {}

        bool operator==(const Color& other) const;
        bool operator!=(const Color& other) const {return !(*this == other);}
        bool operator<(const Color& rhs) const;

        /* Debugging */
        unsigned int GetARGB() const {return a << 24 | r << 16 | g << 8 | b;}
        /** GBA Mode 3's functions */
        static Color FromBGR15(unsigned short color);
        unsigned short GetBGR15() const;
        void GetBGR15(unsigned char& r, unsigned char& g, unsigned char& b) const;
        void SetBGR15(const Color& other);
        void SetBGR15(unsigned short gba_color);

        /** Gets the distance from another color based of perception */
        double Distance(const Color& other) const;

        /** Color components */
        unsigned char r, g, b, a;
};

#endif
