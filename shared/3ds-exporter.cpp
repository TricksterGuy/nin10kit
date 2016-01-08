#include <memory>
#include <set>
#include <vector>

#include "fileutils.hpp"
#include "logger.hpp"
#include "shared.hpp"

void Do3DSExport(const std::vector<Image32Bpp>& images, const std::vector<Image32Bpp>& tilesets, const std::vector<Image32Bpp>& palettes)
{
    if (valid_3ds_modes.find(params.mode) == valid_3ds_modes.end())
    {
        FatalLog("No/Invalid color mode specified image not exported");
    }

    // Add images to header and implementation files
    for (const auto& image : images)
    {
        std::shared_ptr<Exportable> image_ptr(new Image32Bpp(image));
        header.Add(image_ptr);
        implementation.Add(image_ptr);
    }
}
