#ifndef MEDIAN_CUT_HPP
#define MEDIAN_CUT_HPP

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "color.hpp"
#include "histogram.hpp"

class Box
{
    public:
        Box(const Histogram& hist);
        ~Box();
        double Error() const;
        double Volume() const;
        size_t Population() const;
        size_t Size() const;
        void Shrink();
        Box Split();
        Color GetAverageColor() const;
        const Histogram& GetData() const;
        bool operator<(const Box& rhs);
    private:
        Histogram data;
        Color min, max;

};

bool MedianCut(const std::vector<Color>& image, unsigned int desiredColors, std::vector<Color>& palette, const int weights[4]);

#endif

