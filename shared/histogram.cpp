#include "histogram.hpp"
#include <algorithm>
#include <functional>

bool ColorCompare::operator()(Color left, Color right)
{
    bool less;
    switch(index)
    {
        case 0:
            less = left.r < right.r;
            if (left.r == right.r) less = left.g < right.g;
            if (left.r == right.r && left.g == right.g) less = left.b < right.b;
            break;
        case 1:
            less = left.g < right.g;
            if (left.g == right.g) less = left.r < right.r;
            if (left.r == right.r && left.g == right.g) less = left.b < right.b;
            break;
        case 2:
            less = left.b < right.b;
            if (left.b == right.b) less = left.r < right.r;
            if (left.r == right.r && left.b == right.b) less = left.g < right.g;
            break;
        default:
            less = false;
    }
    return less;
}

/** Histogram
  *
  * Creates a histogram from the image.
  */
Histogram::Histogram(const std::vector<Color>& image)
{
    std::vector<Color>::const_iterator i;
    std::map<Color, size_t>::const_iterator j;
    for (i = image.begin(); i != image.end(); ++i)
        data[*i] += 1;
    for (j = data.begin(); j != data.end(); ++j)
        colors.push_back(j->first);
}

/** Histogram
  *
  * Creates a histogram
  */
Histogram::Histogram(Histogram& hist, const std::vector<Color>& keys)
{
    std::copy(keys.begin(), keys.end(), colors.begin());
    std::vector<Color>::const_iterator i;
    for (i = colors.begin(); i != colors.end(); ++i)
        data[*i] = hist.data[*i];
}

/** @brief Histogram
  *
  * @todo: document this function
  */
Histogram::Histogram(const std::map<Color, size_t>& hist, const std::vector<Color>& keys) : data(hist), colors(keys)
{
}

/** @brief GetColors
  *
  * Gets the colors associated with this histogram
  */
const std::vector<Color>& Histogram::GetColors() const
{
    return colors;
}

/** @brief GetData
  *
  * Gets the data associated with this histogram.
  */
const std::map<Color, size_t>& Histogram::GetData() const
{
    return data;
}

/** Population
  *
  * Gets the population of his histogram.
  */
size_t Histogram::Population() const
{
    size_t total = 0;
    std::map<Color, size_t>::const_iterator i;
    for (i = data.begin(); i != data.end(); ++i)
        total += i->second;
    return total;
}

/** @brief Size
  *
  * @todo: document this function
  */
size_t Histogram::Size() const
{
    return colors.size();
}

/** GetAverageColor
  *
  * Gets the average color represented by this histogram.
  */
Color Histogram::GetAverageColor() const
{
    Color out;
    double sumx = 0, sumy = 0, sumz = 0, sump = 0;
    std::map<Color, size_t>::const_iterator i;
    for (i = data.begin(); i != data.end(); ++i)
    {
        Color current = i->first;
        size_t population = i->second;
        sumx += current.r * population;
        sumy += current.g * population;
        sumz += current.b * population;
        sump += population;
    }

    out.r = sumx / sump;
    out.g = sumy / sump;
    out.b = sumz / sump;
    return out;
}

/** @brief Split
  *
  * @todo: document this function
  */
void Histogram::Split(std::map<Color, size_t>& otherData, std::vector<Color>& otherColors, ColorCompare& comp)
{
    size_t population = Population();

    std::sort(colors.begin(), colors.end(), comp);

    // Perform Split finding the median color
    size_t median = population / 2;
    size_t fill = 0;
    while (fill < median)
    {
        Color current = colors[0];
        if (fill + data[current] <= median)
        {
            fill += data[current];
            otherColors.push_back(current);
            otherData[current] = data[current];
            data.erase(current);
            colors.erase(colors.begin());
        }
        else
        {
            // If we can get more than half of this color take all of it. But only if it is not the only color remaining.
            if (median - fill > median / 2 && colors.size() > 1)
            {
                fill += data[current];
                otherColors.push_back(current);
                otherData[current] = data[current];
                data.erase(current);
                colors.erase(colors.begin());
            }
            else
            {
                fill = median;
            }
        }
    }
}
