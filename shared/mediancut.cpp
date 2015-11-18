#include "mediancut.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <set>

#include "dither.hpp"
#include "logger.hpp"

#define R_SCALE 13              /*  scale R (L*) distances by this much  */
#define G_SCALE 24              /*  scale G (a*) distances by this much  */
#define B_SCALE 26              /*  and B (b*) by this much              */

typedef enum {AXIS_UNDEF, AXIS_RED, AXIS_BLUE, AXIS_GREEN} axisType;

typedef std::map<ColorLAB, unsigned long> FrequencyTable;

class ColorRefSet
{
    public:
        ColorRefSet() {}
        void Ref(const Color16& color)
        {
            references[color] += 1;
        }
        void Deref(const Color16& color)
        {
            references[color] -= 1;
            if (references[color] <= 0) references.erase(color);
        }
        size_t Size() const {return references.size();}
        void GetColors(std::vector<Color16>& colors) const
        {
            for (const auto& color_ref : references)
            {
                const auto& color = color_ref.first;
                VerboseLog("Palette - Color(%d %d %d)", color.r, color.g, color.b);
                colors.push_back(color);
            }
        }
    private:
        std::map<Color16, int> references;
};

class Histogram
{
    public:
        Histogram(const std::vector<Color16>& imgdata = std::vector<Color16>());
        unsigned long Size() const {return frequencies.size();}
        bool Empty() const {return frequencies.empty();}
        unsigned long& operator[](const ColorLAB& color) {return frequencies[color];}
        unsigned long& operator[](const Color16& color) {return frequencies[ColorLAB(color)];}
        void Add(const std::vector<Color16>& pixels);
        void Remove(const ColorLAB& color) {frequencies.erase(color);}
        bool Contains(const ColorLAB& color) const {return frequencies.find(color) != frequencies.end();}
        void GetTotals(unsigned long& total, unsigned long& ltotal, unsigned long& atotal, unsigned long& btotal) const;
        FrequencyTable& GetFrequencies() {return frequencies;}
        const FrequencyTable& GetFrequencies() const {return frequencies;}
        void GetColors(std::vector<Color16>& palette) const;
        ColorLAB GetMode() const;
    private:
        FrequencyTable frequencies;
};

class Box
{
    public:
        Box(int lmin, int lmax, int amin, int amax, int bmin, int bmax) : Lmin(lmin), Amin(amin), Bmin(bmin), Lmax(lmax), Amax(amax), Bmax(bmax),
            Lsplit((Lmin + Lmax + 1) / 2), Asplit((Amin + Amax + 1) / 2), Bsplit((Bmin + Bmax + 1) / 2), Lerror(0), Aerror(0), Berror(0) {}
        Box(const Histogram& hist) : histogram(hist), Lmin(0), Amin(0), Bmin(0), Lmax(255), Amax(255), Bmax(255),
            Lsplit((Lmin + Lmax + 1) / 2), Asplit((Amin + Amax + 1) / 2), Bsplit((Bmin + Bmax + 1) / 2), Lerror(0), Aerror(0), Berror(0) {}

        void Update(int boxes_left);
        const Color& GetColor() const {return color;}
        void Split(Box& box, axisType which_axis);

        Histogram histogram;
        int Lmin, Amin, Bmin;
        int Lmax, Amax, Bmax;
        int Lsplit, Asplit, Bsplit;
        uint64_t Lerror, Aerror, Berror;
    private:
        void UpdateError();
        void UpdateSplits(int boxes_left);
        void UpdateColor();
        ColorLAB labColor;
        Color color;
};

Histogram::Histogram(const std::vector<Color16>& imgdata)
{
    for (const auto& color : imgdata)
        frequencies[ColorLAB(color)]++;
}

void Histogram::Add(const std::vector<Color16>& pixels)
{
    for (const auto& color : pixels)
        frequencies[ColorLAB(color)]++;
}

void Histogram::GetTotals(unsigned long& total, unsigned long& ltotal, unsigned long& atotal, unsigned long& btotal) const
{
    for (const auto& color_freq : frequencies)
    {
        const auto& color = color_freq.first;
        unsigned long this_freq = color_freq.second;

        ltotal += color.l * this_freq;
        atotal += color.a * this_freq;
        btotal += color.b * this_freq;
        total += this_freq;
    }
}

void Histogram::GetColors(std::vector<Color16>& colors) const
{
    for (const auto& color_freq : frequencies)
        colors.push_back(Color16(color_freq.first));
}

ColorLAB Histogram::GetMode() const
{
    const ColorLAB* mode = NULL;
    unsigned long max = 0;

    for (const auto& color_freq : frequencies)
    {
        const auto& color = color_freq.first;
        unsigned long this_freq = color_freq.second;

        if (this_freq > max)
        {
            max = this_freq;
            mode = &color;
        }
    }

    return (mode != NULL) ? *mode : ColorLAB();
}

void Box::Update(int boxes_left)
{
    // Swap so that the mins and maxes will change after the for loop.
    std::swap(Lmin, Lmax);
    std::swap(Amin, Amax);
    std::swap(Bmin, Bmax);

    for (const auto& color_freq : histogram.GetFrequencies())
    {
        const auto& color = color_freq.first;
        Lmin = std::min(color.l, Lmin);
        Lmax = std::max(color.l, Lmax);
        Amin = std::min(color.a, Amin);
        Amax = std::max(color.a, Amax);
        Bmin = std::min(color.b, Bmin);
        Bmax = std::max(color.b, Bmax);
    }

    UpdateSplits(boxes_left);
    UpdateColor();
    UpdateError();
}

void Box::UpdateError()
{
    ColorLAB& c = labColor;

    /* Now scan remaining volume of box and compute population */
    Lerror = 0;
    Aerror = 0;
    Berror = 0;

    for (const auto& color_freq : histogram.GetFrequencies())
    {
        const auto& color = color_freq.first;
        unsigned long freq_here = color_freq.second;

        Lerror += freq_here * (c.l - color.l) * (c.l - color.l);
        Aerror += freq_here * (c.a - color.a) * (c.a - color.a);
        Berror += freq_here * (c.b - color.b) * (c.b - color.b);
    }

    Lerror *= R_SCALE * R_SCALE;
    Aerror *= G_SCALE * G_SCALE;
    Berror *= B_SCALE * B_SCALE;
}

void Box::UpdateSplits(int boxes_left)
{
    Lsplit = (Lmax + Lmin) / 2;
    Asplit = (Amax + Amin) / 2;
    Bsplit = (Bmax + Bmin) / 2;

    if (Lsplit == Lmax) Lsplit = Lmin;
    if (Asplit == Amax) Asplit = Amin;
    if (Bsplit == Bmax) Bsplit = Bmin;
}

void lin_to_rgb(const double hr, const double hg, const double hb, unsigned char *r, unsigned char *g, unsigned char *b);

void Box::UpdateColor()
{
    unsigned long total = 0;
    unsigned long ltotal = 0;
    unsigned long atotal = 0;
    unsigned long btotal = 0;

    histogram.GetTotals(total, ltotal, atotal, btotal);

    double l = (double)ltotal / total;
    double a = (double)atotal / total;
    double b = (double)btotal / total;

    labColor.l = (unsigned char) l;
    labColor.a = (unsigned char) a;
    labColor.b = (unsigned char) b;

    lin_to_rgb(l, a, b, &color.r, &color.g, &color.b);
}

void Box::Split(Box& b2, axisType which_axis)
{
    int lb;
    switch (which_axis)
    {
        case AXIS_RED:
            lb = Lsplit;
            Lmax = lb;
            b2.Lmin = lb+1;
            break;
        case AXIS_GREEN:
            lb = Asplit;
            Amax = lb;
            b2.Amin = lb+1;
            break;
        case AXIS_BLUE:
            lb = Bsplit;
            Bmax = lb;
            b2.Bmin = lb+1;
            break;
        default:
            FatalLog("Code error incorrect axis %d, this shouldn't happen", which_axis);
    }

    std::vector<ColorLAB> del;
    for (const auto& color_freq : histogram.GetFrequencies())
    {
        const auto& color = color_freq.first;
        if (color.l > Lmax || color.a > Amax || color.b > Bmax)
        {
            b2.histogram[color] = color_freq.second;
            del.push_back(color);
        }
    }

    for (const auto& color : del)
        histogram.Remove(color);
}

#define BIAS_FACTOR 2.66
#define BIAS_NUMBER 2.0
static Box* find_split_candidate(const std::list<Box>& boxlist, const int desired_colors, axisType *which_axis)
{
    Box* which = NULL;

    double Lbias = 1.0F;
    if (desired_colors <= 16)
    {
        double numboxes = boxlist.size();
        /* we bias towards splitting across L* for first few colors */
        Lbias = (numboxes > BIAS_NUMBER) ? 1.0F : (BIAS_NUMBER - numboxes + 1) * BIAS_FACTOR / BIAS_NUMBER;
    }

    double maxc = 0;
    *which_axis = AXIS_UNDEF;
    for (auto& box : boxlist)
    {
        double lpe = box.Lerror * Lbias;
        double ape = box.Aerror;
        double bpe = box.Berror;
        if (lpe > maxc && box.Lmin < box.Lmax)
        {
            which = const_cast<Box*>(&box);
            maxc = Lbias * lpe;
            *which_axis = AXIS_RED;
        }

        if (ape > maxc && box.Amin < box.Amax)
        {
            which = const_cast<Box*>(&box);
            maxc = ape;
            *which_axis = AXIS_GREEN;
        }

        if (bpe > maxc && box.Bmin < box.Bmax)
        {
            which = const_cast<Box*>(&box);
            maxc = bpe;
            *which_axis = AXIS_BLUE;
        }
    }

    return which;
}

#define LUMINANCE(r, g, b) (0.2126 * (r) + 0.7152 * (g) + 0.0722 * (b))

class PaletteSort
{
    public:
        bool operator()(const Color16& lhs, const Color16& rhs) const
        {
            return LUMINANCE(lhs.r, lhs.g, lhs.b) < LUMINANCE(rhs.r, rhs.g, rhs.b);
        }
};

bool MedianCut(Histogram& hist, unsigned int desired_colors, std::vector<Color16>& palette)
{
    EventLog l(__func__);

    if (hist.Size() <= desired_colors)
    {
        palette.reserve(hist.Size());
        hist.GetColors(palette);
        return false;
    }

    ColorRefSet refset;
    std::list<Box> boxlist;

    boxlist.emplace_back(hist);
    Box& first = boxlist.back();
    first.Update(desired_colors);

    refset.Ref(first.GetColor());

    // Each step we are adding either 0, 1, or 2 colors
    while (refset.Size() < desired_colors)
    {
        axisType which_axis;

        Box* b = find_split_candidate(boxlist, desired_colors, &which_axis);
        if (b == NULL)
            break;

        Box& b1 = *b;
        boxlist.emplace_back(b1.Lmin, b1.Lmax, b1.Amin, b1.Amax, b1.Bmin, b1.Bmax);
        Box& b2 = boxlist.back();
        refset.Deref(b1.GetColor());

        b1.Split(b2, which_axis);

        b1.Update(desired_colors - boxlist.size());
        b2.Update(desired_colors - boxlist.size());

        refset.Ref(b1.GetColor());
        refset.Ref(b2.GetColor());
    }

    refset.GetColors(palette);
    // Could be the case we get n + 1 colors due to two new colors being added.
    palette.resize(desired_colors);

    return true;
}

void GetPalette(const std::vector<Color16>& pixels, unsigned int num_colors, const Color16& transparent, unsigned int offset, Palette& palette)
{
    EventLog l(__func__);

    //BUG HERE
    Histogram hist(pixels);

    std::vector<Color16> paletteArray;
    paletteArray.reserve(num_colors);

    if (!offset)
    {
        num_colors = std::max(1U, num_colors - 1);
        hist.Remove(ColorLAB(transparent));
    }

    MedianCut(hist, num_colors, paletteArray);

    std::sort(paletteArray.begin(), paletteArray.end(), PaletteSort());
    if (!offset)
    {
        const auto& it = std::find(paletteArray.begin(), paletteArray.end(), transparent);
        if (it == paletteArray.end())
            paletteArray.insert(paletteArray.begin(), transparent);
        else
            std::swap(*paletteArray.begin(), *it);
    }

    palette.Set(paletteArray);
}

void GetPalette(const Image16Bpp& image, unsigned int num_colors, const Color16& transparent, unsigned int offset, Palette& palette)
{
    GetPalette(image.pixels, num_colors, transparent, offset, palette);
}

void GetPalette(const std::vector<Image16Bpp>& images, unsigned int num_colors, const Color16& transparent, unsigned int offset, Palette& palette)
{
    EventLog l(__func__);

    Histogram hist;
    for (const auto& image : images)
        hist.Add(image.pixels);

    std::vector<Color16> paletteArray;
    paletteArray.reserve(num_colors);

    if (!offset)
    {
        num_colors = std::max(1U, num_colors - 1);
        hist.Remove(ColorLAB(transparent));
    }

    MedianCut(hist, num_colors, paletteArray);

    std::sort(paletteArray.begin(), paletteArray.end(), PaletteSort());
    if (!offset)
    {
        const auto& it = std::find(paletteArray.begin(), paletteArray.end(), transparent);
        if (it == paletteArray.end())
            paletteArray.insert(paletteArray.begin(), transparent);
        else
            std::swap(*paletteArray.begin(), *it);
    }

    palette.Set(paletteArray);
    VerboseLog("palette array size %d num_colors %d", paletteArray.size(), num_colors);
}

void DitherAndReduceImage(const Image16Bpp& image, const Color16& transparent, bool dither, double dither_level, unsigned int offset, Image8Bpp& indexedImage)
{
    EventLog l(__func__);
    RiemersmaDither(image, indexedImage, transparent, dither, dither_level);
    if (offset > 0)
    {
        for (unsigned int i = 0; i < indexedImage.pixels.size(); i++)
        {
            unsigned char& index = indexedImage.pixels[i];
            const Color16& color = image.pixels[i];
            if (color != transparent)
                index += offset;
        }
    }
}

void ReduceImage(const std::vector<Color16>& pixels, const Palette& palette, const Color16& transparent, unsigned int offset, std::vector<unsigned char>& indexedPixels)
{
    EventLog l(__func__);
    indexedPixels.reserve(pixels.size());
    for (const auto& color : pixels)
        indexedPixels.push_back((color == transparent) ? 0 : palette.Search(color) + offset);
}
