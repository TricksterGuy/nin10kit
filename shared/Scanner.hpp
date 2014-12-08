#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <wx/tokenzr.h>

class Scanner : public wxStringTokenizer
{
    public:
        Scanner(const std::string& str, const std::string& delims = " ,\t\r\n") : wxStringTokenizer(str, delims, wxTOKEN_STRTOK) {}
        ~Scanner() {}
        bool Next(int32_t& var, int base = 10);
        bool Next(uint32_t& var, int base = 10);
        bool Next(float& var);
        bool Next(std::string& var);
        bool NextLine(std::string& var);
};

#endif
