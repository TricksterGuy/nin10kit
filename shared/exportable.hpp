#ifndef EXPORTABLE_HPP
#define EXPORTABLE_HPP

/** Base class that represents an object that can be exported to a c/h file. */
class Exportable
{
    public:
        Exportable(const std::string& _name = "") : name(_name) {}
        virtual ~Exportable() {};
        /** Writes array and other declarations */
        virtual void WriteData(std::ostream& file) const = 0;
        /** Writes externs and other defines */
        virtual void WriteExport(std::ostream& file) const = 0;
        /** Symbol base name (image name) */
        std::string name;
};

#endif
