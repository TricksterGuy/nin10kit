#ifndef COLOR_HPP
#define COLOR_HPP

///TODO rewrite this entire class.
class Color
{
    public:
        Color() : x(0), y(0), z(0) {};
        Color(double a, double b, double c) : x(a), y(b), z(c) {};
        explicit Color(unsigned short color_data);
        bool operator<(const Color& rhs) const;
        void Set(int a, int b, int c);
        unsigned short GetBGR15() const;
        void SetBGR15(unsigned short gba_color);
        double Distance(const Color& other) const;
        double x, y, z;
};

#endif
