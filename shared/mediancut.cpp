#include "mediancut.hpp"

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <queue>

class BoxCompare;

static void CutBoxes(std::priority_queue<Box, std::vector<Box>, BoxCompare>& queue, std::list<Box>& removed, unsigned int desiredColors);
static void SwapQueues(std::priority_queue<Box, std::vector<Box>, BoxCompare>& q1, std::priority_queue<Box, std::vector<Box>, BoxCompare >& q2);

class BoxCompare
{
    public:
        BoxCompare(int mode)
        {
            this->mode = mode;
        }
        bool operator()(Box lhs, Box rhs)
        {
            ///TODO verify this
            bool less;
            switch(mode)
            {
                case 0:
                    less = lhs.Volume() < rhs.Volume();
                    break;
                case 1:
                    less = lhs.Population() * lhs.Size() < rhs.Population() * lhs.Size();
                    break;
                case 2:
                    less = lhs.Population() * lhs.Volume() < rhs.Population() * rhs.Volume();
                    break;
                case 3:
                    less = lhs.Error() < rhs.Error();
                    break;
                default:
                    less = false;
            }
            return less;
        }
        int mode;
};

/** Box
  *
  * Creates a new Box
  */
Box::Box(const Histogram& hist) : data(hist), min(DBL_MAX, DBL_MAX, DBL_MAX), max(DBL_MIN, DBL_MIN, DBL_MIN)
{
}

/** ~Box
  *
  * Destructor
  */
Box::~Box()
{
}

/** @brief GetData
  *
  * @todo: document this function
  */
const Histogram& Box::GetData() const
{
    return data;
}

/** @brief Split
  *
  * @todo: document this function
  */
Box Box::Split()
{
    std::vector<Color> otherColors;
    std::map<Color, size_t> otherHist;

    double x, y, z;
    ColorCompare comp(0);
    x = max.x - min.x;
    y = max.y - min.y;
    z = max.z - min.z;

    if (x > y && x > z)
        comp = ColorCompare(0);
    else if (y > x && y > z)
        comp = ColorCompare(1);
    else
        comp = ColorCompare(2);

    data.Split(otherHist, otherColors, comp);

    return Box(Histogram(otherHist, otherColors));
}

/** @brief Shrink
  *
  * Shrinks the Box
  */
void Box::Shrink()
{
    std::vector<Color>::const_iterator i;
    const std::vector<Color>& colors = data.GetColors();
    for (i = colors.begin(); i != colors.end(); ++i)
    {
        Color c = *i;

        min.x = std::min(c.x, min.x);
        max.x = std::max(c.x, max.x);
        min.y = std::min(c.y, min.y);
        max.y = std::max(c.y, max.y);
        min.z = std::min(c.z, min.z);
        max.z = std::max(c.z, max.z);
    }
}

/** Population
  *
  * Gets the Population of this box
  */
size_t Box::Population() const
{
    return data.Population();
}

/** @brief Size
  *
  * @todo: document this function
  */
size_t Box::Size() const
{
    return data.Size();
}

/** @brief Volume
  *
  * @todo: document this function
  */
double Box::Volume() const
{
    return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
}

/** @brief Error
  *
  * Calculates the Error associated with this box.
  */
double Box::Error() const
{
    Color average = data.GetAverageColor();
    const std::map<Color, size_t>& hist = data.GetData();
    const std::vector<Color> colors = data.GetColors();
    std::vector<Color>::const_iterator i;

    double error = 0;
    for (i = colors.begin(); i != colors.end(); ++i)
    {
        Color color = *i;
        size_t population = hist.find(color)->second;
        error += (color.x - average.x) * (color.x - average.x) * population;
        error += (color.y - average.y) * (color.y - average.y) * population;
        error += (color.z - average.z) * (color.z - average.z) * population;
    }

    return error;
}

/** @brief GetAverageColor
  *
  * @todo: document this function
  */
Color Box::GetAverageColor() const
{
    return data.GetAverageColor();
}

bool MedianCut(const std::vector<Color>& image, unsigned int desiredColors, std::vector<Color>& palette, const int weights[4])
{
    Histogram hist(image);
    // If we have fewer colors than desired
    if (hist.Size() <= desiredColors)
    {
        const std::vector<Color>& colors = hist.GetColors();
        for (const auto& color : colors)
            palette.push_back(color);
        return false;
    }
    // Volume Queue
    std::priority_queue<Box, std::vector<Box>, BoxCompare> queue0(BoxCompare(0));
    // Population Queue
    std::priority_queue<Box, std::vector<Box>, BoxCompare> queue1(BoxCompare(1));
    // Popular Volume Queue
    std::priority_queue<Box, std::vector<Box>, BoxCompare> queue2(BoxCompare(2));
    // Error Queue
    std::priority_queue<Box, std::vector<Box>, BoxCompare> queue3(BoxCompare(3));
    std::list<Box> removed;

    Box box(hist);
    box.Shrink();
    queue0.push(box);



    CutBoxes(queue0, removed, weights[0] * desiredColors / 100);
    SwapQueues(queue0, queue1);
    CutBoxes(queue1, removed, (weights[0] + weights[1]) * desiredColors / 100);
    SwapQueues(queue1, queue2);
    CutBoxes(queue2, removed, (weights[0] + weights[1] + weights[2]) * desiredColors / 100);
    SwapQueues(queue2, queue3);
    CutBoxes(queue3, removed, desiredColors);

    while (!queue3.empty())
    {
        Box current = queue3.top();
        queue3.pop();
        Color color = current.GetAverageColor();
        palette.push_back(color);
    }

    while (!removed.empty())
    {
        Box current = removed.front();
        removed.pop_front();

        Color color = current.GetAverageColor();
        palette.push_back(color);
    }

    return true;
}


void CutBoxes(std::priority_queue<Box, std::vector<Box>, BoxCompare>& queue,
              std::list<Box>& removed, unsigned int desiredColors)
{
    while (queue.size() + removed.size() < desiredColors && !queue.empty())
    {
        Box current = queue.top();
        Histogram histogram = current.GetData();
        queue.pop();

        Box other = current.Split();

        other.Shrink();
        current.Shrink();

        if (current.Size() > 1) queue.push(current);
        if (other.Size() > 1) queue.push(other);
        if (current.Size() == 1) removed.push_back(current);
        if (other.Size() == 1) removed.push_back(other);
    }
}

void SwapQueues(std::priority_queue<Box, std::vector<Box>, BoxCompare>& q1,
                std::priority_queue<Box, std::vector<Box>, BoxCompare>& q2)
{
    while (!q1.empty())
    {
        q2.push(q1.top());
        q1.pop();
    }
}
