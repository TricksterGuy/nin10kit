#ifndef HEADER_FILE_HPP
#define HEADER_FILE_HPP

#include "exportfile.hpp"

#include <iostream>
#include <string>

class HeaderFile : public ExportFile
{
    public:
        HeaderFile(){};
        ~HeaderFile() {};
        virtual void Write(std::ostream& file);
};

extern HeaderFile header;

#endif
