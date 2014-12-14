#ifndef LUT_GEN_HPP
#define LUT_GEN_HPP

#include <map>
#include <string>
#include <cstdint>

#include "exportable.hpp"

#define PI 3.14159265358979323846

typedef double (*MathFunction)(double);

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
        double operator()(double value, bool in_degrees = false);
    private:
        MathFunction function;
        bool input_radians;
        bool output_radians;
};

extern const std::map<std::string, LutFunc> function_map;

/** Class for specifying input/output types for look up table */
class LutIOType
{
    public:
        /** Constructor
          * @param type String in format {type.fixed_length or type or float or double}
          */
        LutIOType(const std::string& type);
    private:
        /** Type @see LutType enum */
        unsigned int type;
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
        std::string name;
        LutFunc function;
        bool in_degrees;
};

class FixedLutGenerator : public LutGenerator
{
    public:
        FixedLutGenerator(const std::string& name, const std::string& function, int64_t begin, int64_t end, int64_t step,
                          const std::string& input = "int", const std::string& output = "short8", bool in_degrees = false);
        ~FixedLutGenerator();
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
    private:
        int64_t begin;
        int64_t end;
        int64_t step;
        // TTFFFFFF
        // Most significant 2 bits are type (0: char, 1: short 2: int 3: long)
        // Least significant 6 bits are fixed_length
        unsigned char input_type;
        unsigned char output_type;
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
