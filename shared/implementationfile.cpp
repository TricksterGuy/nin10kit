#include "implementationfile.hpp"

#include <map>
#include <vector>

#include "fileutils.hpp"
#include "shared.hpp"

ImplementationFile implementation;

void ImplementationFile::Write(std::ostream& file)
{
    ExportFile::Write(file);

    WriteInclude(file, params.export_file + ".h");
    WriteNewLine(file);

    std::map<std::string, std::vector<Image*>> name_frames = GetAnimatedImages();
    for (unsigned int i = 0; i < params.names.size(); i++)
    {
        const std::string& name = params.names[i];
        unsigned int frames = name_frames[name].size();
        if (frames <= 1) continue;
        std::vector<std::string> names;
        const std::string& img_type = name_frames[name][0]->GetImageType();
        for (const auto& image : name_frames[name])
            names.push_back(image->GetExportName());
        WriteAnimationArray(file, img_type, name, "_frames", names, 1);
        WriteNewLine(file);
    }

    for (const auto& exportable : exportables)
        exportable->WriteData(file);
}
