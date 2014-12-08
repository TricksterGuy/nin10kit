#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include <cstdlib>
#include <map>
#include <vector>

#include "color.hpp"

class ColorCompare
{
    public:
        ColorCompare(int _index) : index(_index) {};
        bool operator()(Color left, Color right);
        int index;
};

class Histogram
{
    public:
        Histogram(const std::vector<Color>& image);
        Histogram(Histogram& hist, const std::vector<Color>& keys);
        Histogram(const std::map<Color, size_t>& hist, const std::vector<Color>& keys);
        size_t Population() const;
        size_t Size() const;
        const std::map<Color, size_t>& GetData() const;
        const std::vector<Color>& GetColors() const;
        Color GetAverageColor() const;
        void Split(std::map<Color, size_t>& otherData, std::vector<Color>& otherColors, ColorCompare& comp);
    private:
        std::map<Color, size_t> data;
        std::vector<Color> colors;
};

#endif
