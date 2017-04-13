#include "lutgen.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

#include "export_params.hpp"
#include "fileutils.hpp"
#include "logger.hpp"
#include "scanner.hpp"
#include "shared.hpp"

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

const std::map<std::string, LutType> type_map = {
    {"char",  LutType::CHAR},
    {"byte",  LutType::CHAR},
    {"short", LutType::SHORT},
    {"int",   LutType::INT},
    {"long",  LutType::LONG},
};

const std::map<LutType, std::string> type_map_rev = {
    {LutType::CHAR, "char"},
    {LutType::SHORT, "short"},
    {LutType::INT, "int"},
    {LutType::LONG, "long"},
};

std::string form_fixed_string(int64_t fixed, const LutIOType& lut_type)
{
    std::stringstream str;
    int pad = (lut_type.GetLength() + 3) / 4;
    if (pad == 0) pad = 1;
    uint64_t mask = (1L << lut_type.GetLength()) - 1;
    if (lut_type.GetLength() > 0)
        str << "((" << (fixed >> lut_type.GetLength()) << " << " << lut_type.GetLength() << ") | 0x" << std::setfill('0')
            << std::setw(pad) << std::hex << (fixed & mask) << ")";
    else
        str << fixed;
    return str.str();
}

LutSpecification::LutSpecification(const std::string& spec)
{
    Scanner scan(spec, ",");
    if (!scan.Next(function))
        FatalLog("Could not parse function name %s", spec.c_str());
    if (!scan.Next(type))
        FatalLog("Could not parse type %s", spec.c_str());
    if (!scan.Next(begin))
        FatalLog("Could not parse begin %s", spec.c_str());
    if (!scan.Next(end))
        FatalLog("Could not parse end %s", spec.c_str());
    if (!scan.Next(step))
        FatalLog("Could not parse step %s", spec.c_str());
    if (scan.HasMoreTokens())
    {
        if (!scan.Next(in_degrees))
            FatalLog("Could not parse in_degrees %s", spec.c_str());
    }
}

std::string LutSpecification::str() const
{
    std::stringstream out;
    out << "Function: " << function << " Type: " << type << " Range: [" << begin << ", " << end << "] Step: " << step;
    return out.str();
}


LutFunc::LutFunc(MathFunction func, bool in_radians, bool out_radians) : function(func), input_radians(in_radians), output_radians(out_radians)
{
}

double LutFunc::operator()(double value, bool in_degrees) const
{
    // If function accepts radians and value given is in degrees
    if (input_radians && in_degrees)
        value *= PI / 180;
    double out = function(value);
    // If function outputs radians and value we want to be in degrees
    if (output_radians && in_degrees)
        out *= 180 / PI;
    return out;
}

LutIOType::LutIOType(const std::string& type_spec) : fixed_length(0)
{
    Scanner scan(type_spec, ".");
    std::string type_str;

    if (!scan.Next(type_str))
        FatalLog("Unable to parse type string %s", type_spec.c_str());

    if (scan.HasMoreTokens())
    {
        if (!scan.Next(fixed_length))
            FatalLog("Unable to parse type string %s", type_str.c_str());
    }

    if (type_map.find(type_str) == type_map.end())
        FatalLog("Valid data types are char, byte, short, int, and long. %s given", type_str.c_str());
    type = type_map.at(type_str);

    unsigned int num_bits = 1 << ((unsigned int)type + 3);
    if (fixed_length >= num_bits)
        FatalLog("Invalid number given for fixed point %d given >= %d bits", fixed_length, num_bits);
}

double LutIOType::ConvertToDouble(int64_t fixed) const
{
    ///TODO fix this
    double answer = fixed >> fixed_length;
    int64_t fractional = fixed & ((1 << fixed_length) - 1);
    double current = 1.0;
    for (int i = fixed_length; i >= 0; i--)
    {
        current /= 2;
        if (fractional & (1 << i))
            answer += current;
    }
    return answer;
}

int64_t LutIOType::ConvertToFixed(double fpoint) const
{
    bool negate = fpoint < 0;
    fpoint = fabs(fpoint);
    int64_t integral = ((int64_t)fpoint) << fixed_length;
    int64_t fractional = 0;
    // fpoint should be a decimal
    fpoint -= (int64_t)fpoint;
    double current = 1.0;
    for (unsigned int i = 0; i < fixed_length; i++)
    {
        fractional <<= 1;
        current /= 2;
        if (fpoint >= current)
        {
            fpoint -= current;
            fractional |= 1;
        }
    }
    uint64_t num_bits = 1 << ((unsigned int)type + 3);
    int64_t answer = integral | fractional;
    if (negate) answer = (-answer) & ((1L << num_bits) - 1);
    return answer;
}


LutGenerator::LutGenerator(const std::string& name, const std::string& function_name, bool _in_degrees) : Exportable(name), in_degrees(_in_degrees)
{
    if (function_map.find(function_name) == function_map.end())
        FatalLog("No function named %s can not export LUT", function_name.c_str());

    function = function_map.at(function_name);
}

FixedLutGenerator::FixedLutGenerator(const std::string& name, const std::string& function, double _begin, double _end, double _step,
                                     const std::string& output, bool in_degrees) :
                                     LutGenerator(name, function, in_degrees), type(output), begin(_begin), end(_end), step(_step)
{
    if (step == 0)
        FatalLog("Step must be a nonzero value");
    if (begin >= end)
        FatalLog("Begin must be less than end");
    if ((end - begin) / step > LUT_ENTRIES_MAX && !params.force)
        FatalLog("Will generate %d entries in look up table. If you are sure about this use -force.", end - begin / step);
}

FixedLutGenerator::~FixedLutGenerator()
{

}

void FixedLutGenerator::WriteExport(std::ostream& file) const
{
    unsigned long num_entries = (end - begin) / step + 1;
    WriteExtern(file, "const " + type_map_rev.at(type.GetType()), name, "", num_entries);
    WriteDefine(file, name, "_LENGTH", num_entries);
    WriteDefine(file, name, "_FIXED_LENGTH", type.GetLength());
    WriteDefine(file, name, "_BEGIN", form_fixed_string(type.ConvertToFixed(begin), type));
    WriteDefine(file, name, "_END", form_fixed_string(type.ConvertToFixed(end), type));
    WriteDefine(file, name, "_STEP", form_fixed_string(type.ConvertToFixed(step), type));
    WriteNewLine(file);
}

void FixedLutGenerator::WriteData(std::ostream& file) const
{
    unsigned int size = (end - begin) / step + 1;
    double current = begin;
    file << "const " << type_map_rev.at(type.GetType()) << " " << name << "[" << size << "] =\n{\n\t";
    for (unsigned int i = 0; i < size; i++)
    {
        current = fma(i, step, begin);
        double out = function(current, in_degrees);
        int64_t out_value = type.ConvertToFixed(out);
        VerboseLog("%s(%f) => %f => %ld in_degrees:%d", name.c_str(), current, out, out_value, in_degrees);
        WriteElement(file, form_fixed_string(out_value, type), size, i, 10);
    }
    file << "\n};\n";
    WriteNewLine(file);
}
