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

        header.AddLutInfo(spec);
        implementation.AddLutInfo(spec);

        LutGenerator* generator = new FixedLutGenerator(name, spec.function, spec.begin, spec.end, spec.step, spec.type, spec.in_degrees);
        std::shared_ptr<Exportable> exportable(generator);
        implementation.Add(exportable);
        header.Add(exportable);
    }
}
