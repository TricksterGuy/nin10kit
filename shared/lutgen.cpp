#include "lutgen.hpp"

#include <cmath>
#include "scanner.hpp"
#include "logger.hpp"

const std::map<std::string, LutFunc> function_map =
{
    {"cos", LutFunc(cos, true)},
    {"sin", LutFunc(sin, true)},
    {"tan", LutFunc(tan, true)},
    {"acos", LutFunc(acos, false, true)},
    {"asin", LutFunc(asin, false, true)},
    {"atan", LutFunc(atan, false, true)},
    {"cosh", LutFunc(cosh, true)},
    {"sinh", LutFunc(sinh, true)},
    {"tanh", LutFunc(tanh, true)},
    {"acosh", LutFunc(acosh, false, true)},
    {"asinh", LutFunc(asinh, false, true)},
    {"atanh", LutFunc(atanh, false, true)},
    {"exp", exp},
    {"log", log},
    {"log10", log10},
    {"log2", log2},
    {"exp2", exp2},
    {"expm1", expm1},
    {"log1p", log1p},
    {"sqrt", sqrt},
    {"cbrt", cbrt},
    {"erf", erf},
    {"erfc", erfc},
    {"tgamma", tgamma},
    {"lgamma", lgamma},
};

const std::map<std::string, int> type_map = {
    {"char",    0},
    {"byte",    0},
    {"short",   1},
    {"int",     2},
    {"long",    3},
};

unsigned char parse_fixed_type(const std::string& type_str)
{
    Scanner scan(type_str, ".");
    std::string type;
    int size;
    int fixed_size = 0;

    if (!scan.Next(type))
        FatalLog("Unable to parse type string %s", type_str.c_str());

    if (scan.HasMoreTokens())
    {
        if (!scan.Next(fixed_size))
            FatalLog("Unable to parse type string %s", type_str.c_str());
    }

    if (type_map.find(type) == type_map.end())
        FatalLog("Valid data types are char, byte, short, int, and long. %s given", type.c_str());
    size = type_map.at(type);

    int num_bits = 2 << (size + 3);
    if (fixed_size >= num_bits);
        FatalLog("Invalid number given for fixed point %d given >= %d bits", fixed_size, num_bits);

    return size << 6 | fixed_size;
}

LutFunc::LutFunc(MathFunction func, bool in_radians, bool out_radians) : function(func), input_radians(in_radians), output_radians(out_radians)
{
}

double LutFunc::operator()(double value, bool in_degrees)
{
    // If function accepts radians and value given is in degrees
    if (input_radians && in_degrees)
        value *= 180 / PI;
    double out = function(value);
    // If function outputs radians and value we want to be in degrees
    if (output_radians && in_degrees)
        out *= 180 / PI;
    return out;
}

LutGenerator::LutGenerator(const std::string& _name, const std::string& function_name, bool _in_degrees) : name(_name), in_degrees(_in_degrees)
{
    if (function_map.find(function_name) == function_map.end())
        FatalLog("No function named %s can not export LUT", function_name.c_str());

    function = function_map.at(function_name);
}

FixedLutGenerator::FixedLutGenerator(const std::string& name, const std::string& function, int64_t _begin, int64_t _end, int64_t _step,
                                     const std::string& input, const std::string& output, bool in_degrees) :
                                     LutGenerator(name, function, in_degrees), begin(_begin), end(_end), step(_step)
{
    input_type = parse_fixed_type(input);
    output_type = parse_fixed_type(output);
}

FixedLutGenerator::~FixedLutGenerator()
{

}

void FixedLutGenerator::WriteData(std::ostream& file) const
{

}

void FixedLutGenerator::WriteExport(std::ostream& file) const
{

}


