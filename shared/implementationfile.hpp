#ifndef IMPLEMENTATION_FILE_HPP
#define IMPLEMENTATION_FILE_HPP

#include "exportfile.hpp"

#include <iostream>
#include <string>

class ImplementationFile : public ExportFile
{
    public:
        ImplementationFile() {};
        ~ImplementationFile() {};
        virtual void Write(std::ostream& file);
};

extern ImplementationFile implementation;

#endif

