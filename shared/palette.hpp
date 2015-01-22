#ifndef PALETTE_HPP
#define PALETTE_HPP

#include <Magick++.h>
#include <map>
#include <set>
#include <vector>

#include "color.hpp"
#include "exportable.hpp"

struct PaletteEntryStats
{
    PaletteEntryStats() : used_count(0), perfect_count(0), error(0) {}
    int used_count;
    int perfect_count;
    unsigned long error;
};

struct ColorEntryStats
{
    ColorEntryStats() : index(0), count(0), error(0) {}
    int index;
    int count;
    unsigned long error;
};

/** Base class for palettes/palette banks.  Represents a set of colors. */
class ColorArray
{
    public:
        ColorArray(const std::vector<Color16>& _colors = std::vector<Color16>());
        /** Sets array to contain colors passed in */
        void Set(const std::vector<Color16>& _colors);
        /** Gets color at palette index */
        const Color16& At(int index) const {return colors[index];}
        /** Search palette for color passed in returns the closest palette index that matches the color */
        int Search(const Color16& color) const;
        /** Are all colors contained in the palette? */
        bool Contains(const ColorArray& palette) const;
        /** Adds a color to the palette */
        void Add(const Color16& c);
        /** Gets size of this palette */
        unsigned int Size() const {return colors.size();}
        /** Gets colors in palette */
        const std::vector<Color16> GetColors() const {return colors;}
        mutable std::map<int, PaletteEntryStats> paletteEntryStats;
        /** Cache to speed up repeatedly hit colors */
        mutable std::map<Color16, ColorEntryStats> colorEntryStats;
    protected:
        /** Colors contained in palette with set to prevent duplicates */
        std::vector<Color16> colors;
        std::vector<ColorLAB> labColors;
        std::set<Color16> colorSet;
};

/** Palette class, just an exportable color array */
class Palette : public ColorArray, public Exportable
{
    public:
        Palette(const std::string& name = "") : Exportable(name) {} // Used by Tile constructor
        Palette(const std::vector<Color16>& _colors, const std::string& _name);
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        Magick::Image ToMagick() const;
};

/** Represents a palette bank for 4 bpp exports. */
class PaletteBank : public ColorArray
{
    public:
        PaletteBank(int _id = -1) : id(_id) {};
        PaletteBank(const std::vector<Color16>& _colors, int id);
        void CanMerge(const ColorArray& palette, int& colors_left, int& delta) const;
        void Merge(const ColorArray& palette);
        void BestMerge(const ColorArray& palette);
        unsigned long CalculateError(const ColorArray& palette) const;
        unsigned long CalculateError(const Color16& color) const;
        int id;
};

/** Represents 16 palette banks for 4 bpp exports. */
class PaletteBankManager : public Exportable
{
    public:
        PaletteBankManager(const std::string& _name);
        PaletteBankManager(const std::string& _name, const std::vector<PaletteBank>& paletteBanks);
        PaletteBank& operator[](int i) {return banks[i];}
        const PaletteBank& operator[](int i) const {return banks[i];}
        unsigned int Size() const {return banks.size();}
        int FindBestMatch(const ColorArray& palette) const;
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::vector<PaletteBank> banks;
};

#endif
