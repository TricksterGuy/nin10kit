#include <memory>
#include <vector>

#include "fileutils.hpp"
#include "shared.hpp"

void DoDSExport(const std::vector<Image32Bpp>& images32, const std::vector<Image32Bpp>& tilesets32)
{
    // Add images to header and implementation files
    for (const auto& image : images32)
    {
        std::shared_ptr<Exportable> image_ptr(new Image32Bpp(image));
        header.Add(image_ptr);
        implementation.Add(image_ptr);
    }
}
