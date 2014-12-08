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

    WriteHeaderGuard(file, params.symbol_base_name, "_H");

    /// TODO This needs to change
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
    if (ok_newline) WriteNewLine(file);

    for (const auto& exportable : exportables)
        exportable->WriteExport(file);

    WriteEndHeaderGuard(file);
    WriteNewLine(file);
}
