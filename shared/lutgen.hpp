#ifndef LUT_GEN_HPP
#define LUT_GEN_HPP

#include <map>
#include <string>
#include <cstdint>

#include "exportable.hpp"

#define PI 3.14159265358979323846
#define LUT_ENTRIES_MAX 1000000

typedef double (*MathFunction)(double);

class LutSpecification
{
    public:
        LutSpecification(const std::string& spec);
        std::string function;
        std::string type;
        double begin;
        double end;
        double step;
        int in_degrees;
};

/** Class modeling a function that can be used to generate a look up table */
class LutFunc
{
    public:
        /** Constructor
          * @param func pointer to function that takes in double and returns double
          * @param in_radians true if the function accepts parameters in radians
          * @param out_radians true if function returns values in radians
          */
        LutFunc(MathFunction func = NULL, bool in_radians = false, bool out_radians = false);
        ~LutFunc() {}
        double operator()(double value, bool in_degrees = false) const;
    private:
        MathFunction function;
        bool input_radians;
        bool output_radians;
};

extern const std::map<std::string, LutFunc> function_map;

/** Enum specifying valid LUT types*/
enum class LutType
{
    CHAR = 0,
    SHORT = 1,
    INT = 2,
    LONG = 3,
    FLOAT = 4,
    DOUBLE = 5,
};

/** Class for specifying input/output types for look up table */
class LutIOType
{
    public:
        /** Constructor
          * @param type String in format {type.fixed_length or type or float or double}
          */
        LutIOType(const std::string& type);
        LutType GetType() const {return type;}
        unsigned int GetLength() const {return fixed_length;}
        double ConvertToDouble(int64_t fixed) const;
        int64_t ConvertToFixed(double fpoint) const;
    private:
        /** Type @see LutType enum */
        LutType type;
        /** Number of bits to dedicate to decimal portion */
        unsigned int fixed_length;
};

/** Base class for generating a look up table. */
class LutGenerator : public Exportable
{
    public:
        /** Constructor
          * @param name Name of the generator.
          * @param func_name Function to generate a look up table for
          * @param parameters passed into function are in degrees.
          */
        LutGenerator(const std::string& name, const std::string& func_name, bool in_degrees = false);
        virtual ~LutGenerator() {}
    protected:
        LutFunc function;
        bool in_degrees;
};

/** Look up table generator that deals with generating luts on fixed point types */
class FixedLutGenerator : public LutGenerator
{
    public:
        FixedLutGenerator(const std::string& name, const std::string& function, double begin, double end, double step,
                          const std::string& output = "short8", bool in_degrees = false);
        ~FixedLutGenerator();
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
    private:
        LutIOType type;
        double begin;
        double end;
        double step;
};

class FloatLutGenerator : public LutGenerator
{
    public:
        FloatLutGenerator(const std::string& name, const std::string& function, double begin, double end, double step,
                          const std::string& input = "int", const std::string& output = "short8", bool in_degrees = false);
        ~FloatLutGenerator();
    protected:
    private:
        double begin;
        double end;
        double step;

};


#endif
