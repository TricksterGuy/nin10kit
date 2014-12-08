#include "implementationfile.hpp"

#include <map>
#include <vector>

#include "fileutils.hpp"
#include "shared.hpp"

ImplementationFile implementation;

void ImplementationFile::Write(std::ostream& file)
{
    ExportFile::Write(file);

    for (const auto& exportable : exportables)
        exportable->WriteData(file);
}
