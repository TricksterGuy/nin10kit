#include "palette.hpp"

#include <cstdio>

#include "logger.hpp"
#include "export_params.hpp"
#include "fileutils.hpp"
#include "shared.hpp"

ColorArray::ColorArray(const std::vector<Color16>& _colors) : colors(_colors), colorSet(colors.begin(), colors.end())
{
    labColors.reserve(colors.size());
    for (const auto& color : colors)
        labColors.push_back(ColorLAB(color));
}

void ColorArray::Clear()
{
    labColors.clear();
    colors.clear();
    colorSet.clear();
    paletteEntryStats.clear();
    colorEntryStats.clear();
}

void ColorArray::Set(const std::vector<Color16>& _colors)
{
    colors = _colors;
    colorSet.clear();
    colorSet.insert(colors.begin(), colors.end());
    labColors.reserve(colors.size());
    for (const auto& color : colors)
        labColors.push_back(ColorLAB(color));
}

bool ColorArray::Set(unsigned int index, const Color16& color)
{
    if (index >= 256)
        return false;

    if (index >= colors.size())
    {
        colors.resize(index + 1);
        labColors.resize(index + 1);
        colorSet.insert(colors.begin(), colors.end());
    }

    Color16 old = colors[index];

    if (old == color)
        return true;

    if (colorSet.find(color) != colorSet.end())
        return false;

    colors[index] = color;
    colorSet.erase(old);
    colorSet.insert(color);
    labColors[index] = ColorLAB(color);

    return true;
}

int ColorArray::Search(const Color16& color) const
{
    unsigned long bestd = 0x7FFFFFFF;
    int index = -1;

    if (colorEntryStats.find(color) != colorEntryStats.end())
    {
        int index = colorEntryStats[color].index;
        colorEntryStats[color].count += 1;
        paletteEntryStats[index].used_count += 1;
        paletteEntryStats[index].error += colorEntryStats[color].error;
        if (colorEntryStats[color].error == 0)
            paletteEntryStats[index].perfect_count += 1;
        return index;
    }

    ColorLAB a(color);
    for (unsigned int i = 0; i < labColors.size(); i++)
    {
        const ColorLAB& b = labColors[i];
        unsigned long dist = a.Distance(b);
        if (dist <= bestd)
        {
            index = i;
            bestd = dist;

            if (bestd == 0)
            {
                paletteEntryStats[index].perfect_count += 1;
                break;
            }
        }
    }

    colorEntryStats[color].index = index;
    colorEntryStats[color].count += 1;
    colorEntryStats[color].error = bestd;

    paletteEntryStats[index].used_count += 1;
    paletteEntryStats[index].error += bestd;

    if (bestd != 0)
    {
        VerboseLog("Color remap: Color (%d %d %d) (%d %d %d) given to palette not an exact match. palette entry: %d - (%d %d %d) (%d %d %d).  dist: %ld.",
                   color.r, color.g, color.b, a.l, a.a, a.b, index, colors[index].r, colors[index].g, colors[index].b,
                   labColors[index].l, labColors[index].a, labColors[index].b, bestd);
    }

    return index;
}

bool ColorArray::Contains(const ColorArray& palette) const
{
    for (const auto& color : palette.colors)
    {
        if (colorSet.find(color) == colorSet.end())
            return false;
    }
    return true;
}

void ColorArray::Add(const Color16& c)
{
    if (colorSet.find(c) == colorSet.end())
    {
        colorSet.insert(c);
        colors.push_back(c);
        labColors.push_back(ColorLAB(c));
    }
}

Palette::Palette(const std::vector<Color16>& colors, const std::string& name) : ColorArray(colors), Exportable(name)
{
    if (colors.size() + params.offset > 256)
        FatalLog("Too many colors in palette. Found %d colors, offset is %d.", colors.size() + params.offset, params.offset);
}

void Palette::WriteData(std::ostream& file) const
{
    WriteBeginArray(file, "const unsigned short", name, "_palette", colors.size());
    WriteColor16Array(file, colors, 8, params.device == "GBA");
    WriteEndArray(file);
    WriteNewLine(file);
}

void Palette::WriteExport(std::ostream& file) const
{
    WriteExtern(file, "const unsigned short", name, "_palette", colors.size());
    WriteDefine(file, name, "_PALETTE_SIZE", colors.size() * 2);
    WriteDefine(file, name, "_PALETTE_LENGTH", colors.size());
    WriteNewLine(file);
}

struct ColorError
{
    ColorError(const Color16& c, unsigned long e) : color(c), error(e) {}
    bool operator<(const ColorError& rhs) const {return error < rhs.error;}
    Color16 color;
    unsigned long error;
};

void PaletteBank::CanMerge(const ColorArray& palette, int& colors_left, int& delta) const
{
    int size = colorSet.size();

    for (const auto& color : palette.GetColors())
        if (colorSet.find(color) == colorSet.end())
            size++;

    colors_left = 16 - size;
    delta = size - colorSet.size();
}

void PaletteBank::Merge(const ColorArray& palette)
{
    for (const auto& color : palette.GetColors())
        Add(color);
}

void PaletteBank::BestMerge(const ColorArray& palette)
{
    // Add the colors that will reduce error the most
    // The rest will be matched.
    int dropX = 16 - colorSet.size();

    std::vector<ColorError> colors;
    for (const auto& color : palette.GetColors())
        colors.push_back(ColorError(color, CalculateError(color)));
    std::sort(colors.begin(), colors.end(), std::less<ColorError>());

    for (int i = 0; i < dropX; i++)
        Add(colors[colors.size() - 1 - i].color);
}

unsigned long PaletteBank::CalculateError(const Color16& color) const
{
    unsigned long min = 0xFFFFFFFF;
    ColorLAB lcolor(color);
    for (const auto& c : labColors)
    {
        unsigned long dist = c.Distance(lcolor);
        if (dist < min)
            min = dist;
    }
    return min;
}

unsigned long PaletteBank::CalculateError(const ColorArray& palette) const
{
    int dropX = 16 - colorSet.size();
    std::vector<unsigned long> errors;
    errors.reserve(palette.Size());

    for (const auto& color : palette.GetColors())
        errors.push_back(CalculateError(color));

    std::sort(errors.begin(), errors.end(), std::less<unsigned long>());

    unsigned long sum = 0;
    for (unsigned int i = 0; i < errors.size() - dropX; i++)
        sum += errors[i];

    return sum;
}

std::ostream& operator<<(std::ostream& file, const PaletteBank& bank)
{
    std::vector<Color16> colors = bank.GetColors();
    colors.resize(16);
    WriteColor16Array(file, colors, 8, params.device == "GBA");
    return file;
}

PaletteBankManager::PaletteBankManager(const std::string& name) : Exportable(name), banks(16)
{
    for (unsigned int i = 0; i < banks.size(); i++)
        banks[i].id = i;
}

PaletteBankManager::PaletteBankManager(const std::string& _name, const std::vector<PaletteBank>& paletteBanks) : Exportable(_name), banks(paletteBanks)
{
}

void PaletteBankManager::Copy(const Palette& palette)
{
    const auto& colors = palette.GetColors();
    banks.resize(16);
    for (auto& bank : banks)
      bank.Clear();

    for (unsigned int i = 0; i < colors.size(); i++)
    {
        banks[i / 16].Add(colors[i]);
    }
}

int PaletteBankManager::FindBestMatch(const ColorArray& palette) const
{
    unsigned long min = 0x7FFFFFFF;
    int index = -1;
    for (unsigned int i = 0; i < banks.size(); i++)
    {
        unsigned long error_dist = banks[i].CalculateError(palette);
        if (error_dist < min)
        {
            min = error_dist;
            index = i;
        }
    }
    return index;
}

void PaletteBankManager::WriteData(std::ostream& file) const
{
    file << "const unsigned short " << name << "_palette[256] =\n{\n\t";
    for (unsigned int i = 0; i < 16; i++)
    {
        file << banks[i];
        if (i != 16 - 1)
            file << ",\n\t";
    }
    file << "\n};\n";
    WriteNewLine(file);
}

void PaletteBankManager::WriteExport(std::ostream& file) const
{
    WriteExtern(file, "const unsigned short", name, "_palette", 256);
    WriteDefine(file, name, "_PALETTE_SIZE", 512);
    WriteDefine(file, name, "_PALETTE_LENGTH", 256);
    WriteNewLine(file);
}
