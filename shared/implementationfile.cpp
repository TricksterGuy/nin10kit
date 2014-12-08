#include "implementationfile.hpp"

#include <map>
#include <vector>

#include "fileutils.hpp"
#include "shared.hpp"

ImplementationFile implementation;

void ImplementationFile::Write(std::ostream& file)
{
    ExportFile::Write(file);
    const char* img_type = params.device == "3DS" ? "const unsigned char*" : "const unsigned short*";

    for (const auto& exportable : exportables)
        exportable->WriteData(file);

    // TODO handle special case of Sprites.  Need offsets instead of ptrs.
    std::map<std::string, std::vector<Image*>> name_frames = GetAnimatedImages();
    for (unsigned int i = 0; i < params.names.size(); i++)
    {
        const std::string& name = params.names[i];
        unsigned int frames = name_frames[name].size();
        if (frames <= 1) continue;
        std::vector<std::string> names;
        for (const auto& image : name_frames[name])
            names.push_back(image->export_name);
        WriteAnimationArray(file, img_type, name, "_frames", names, 1);
    }
}
