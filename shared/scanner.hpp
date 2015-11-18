#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <wx/tokenzr.h>

/** Scanner class wrapping wxStringTokenizer
  * Automatically scans and converts next token to various types
  */
class Scanner : public wxStringTokenizer
{
    public:
        /** Constructor
          * @param str String to use as tokenizer
          * @param delims Delimiters for str
          */
        Scanner(const std::string& str, const std::string& delims = " ,\t\r\n") : wxStringTokenizer(str, delims, wxTOKEN_RET_EMPTY_ALL) {}
        /** Constructor
          * @param str String to use as tokenizer
          * @param delims Delimiters for str
          */
        Scanner(const wxString& str, const wxString& delims = " ,\t\r\n") : wxStringTokenizer(str, delims, wxTOKEN_RET_EMPTY_ALL) {}
        ~Scanner() {}
        /** Reads next integer from Scanner
          * @param var value to store integer in
          * @param base Next token from scanner is treated as an integer in this base
          * @return true if the value was read successfully
          */
        bool Next(int& var, int base = 10);
        /** Reads next unsigned integer from Scanner
          * @param var value to store integer in
          * @param base Next token from scanner is treated as an integer in this base
          * @return true if the value was read successfully
          */
        bool Next(unsigned int& var, int base = 10);
        /** Reads next float from Scanner
          * @param var value to store float in
          * @return true if the value was read successfully
          */
        bool Next(float& var);
        /** Reads next double from Scanner
          * @param var value to store double in
          * @return true if the value was read successfully
          */
        bool Next(double& var);
        /** Reads next string from Scanner
          * @param var value to store string in
          * @return true if the value was read successfully
          */
        bool Next(std::string& var);
        /** Reads next line from Scanner (do not use)
          * @param var value to store string in
          * @return true if the value was read successfully
          */
        bool NextLine(std::string& var);
};

#endif
