#ifndef EXPORTER_HPP
#define EXPORTER_HPP

#include "imageutil.hpp"

enum Mode
{
    GBAMode3,
    GBAMode4,
    GBAMode08Bpp,
    GBAMode04Bpp,
    GBASprites8Bpp,
    GBASprites4Bpp,
};

void GetModeInfo(int prog_mode, std::string& mode, std::string& device, int& bpp);
void DoExport(int mode, const std::string& filename, std::vector<std::string>& filenames, std::map<std::string, ImageInfo>& images);

#endif
