#include "headerfile.hpp"

#include <cstdio>
#include <map>
#include <vector>

#include "fileutils.hpp"
#include "shared.hpp"

HeaderFile header;

void HeaderFile::Write(std::ostream& file)
{
    ExportFile::Write(file);
    const char* img_type = params.device == "3DS" ? "const unsigned char*" : "const unsigned short*";

    WriteHeaderGuard(file, params.symbol_base_name, "_H");

    std::map<std::string, std::vector<Image*>> name_frames = GetAnimatedImages();

    /// TODO This needs to change new devices different datatypes.
    bool ok_newline = false;
    if (transparent_color != -1)
    {
        char buffer[7];
        sprintf(buffer, "0x%04x", transparent_color);
        WriteDefine(file, params.symbol_base_name, "_TRANSPARENT", (mode == "3") ? buffer : "0x00");
        ok_newline = true;
    }
    if (params.offset)
    {
        WriteDefine(file, params.symbol_base_name, "_PALETTE_OFFSET ", params.offset);
        ok_newline = true;
    }
    for (unsigned int i = 0; i < params.names.size(); i++)
    {
        const std::string& name = params.names[i];
        unsigned int frames = name_frames[name].size();
        if (frames <= 1) continue;
        ok_newline = true;
        // Write the common #defines here.
        name_frames[name][0]->WriteCommonExport(file);
        WriteDefine(file, name, "_FRAMES", frames);
        WriteExtern(file, img_type, name, "_frames", frames);

        WriteNewLine(file);
    }
    if (ok_newline) WriteNewLine(file);

    for (const auto& exportable : exportables)
        exportable->WriteExport(file);

    WriteEndHeaderGuard(file);
    WriteNewLine(file);
}
