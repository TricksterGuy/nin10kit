#include <memory>
#include <vector>

#include "export_params.hpp"
#include "fileutils.hpp"
#include "lutgen.hpp"
#include "shared.hpp"

void DoLUTExport(const std::vector<LutSpecification>& functions)
{
    for (unsigned int i = 0; i < functions.size(); i++)
    {
        const auto& spec = functions[i];
        const std::string& name = params.names[i];

        ExportFile::AddLutInfo(spec);
        ExportFile::Add(std::make_unique<FixedLutGenerator>(name, spec.function, spec.begin, spec.end, spec.step, spec.type, spec.in_degrees));
    }
}
