#ifndef CMD_LINE_PARSER_HELPER_HPP
#define CMD_LINE_PARSER_HELPER_HPP

#include <limits>
#include <string>
#include <vector>

#include <wx/cmdline.h>

class CmdLineParserHelper
{
    public:
        CmdLineParserHelper(wxCmdLineParser& parser_) : parser(parser_) {}
        ~CmdLineParserHelper() {}
        bool GetSwitch(const std::string& param);
        int GetInt(const std::string& param, int def_value = 0, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max());
        int GetHexInt(const std::string& param, int def_value = 0, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max());
        long GetLong(const std::string& param, long def_value = 0, long min = std::numeric_limits<long>::min(), long max = std::numeric_limits<long>::max());
        std::string GetString(const std::string& param, const std::string& def_value = "");
        std::vector<int> GetListInt(const std::string& param, const std::vector<int>& def_value = std::vector<int>());
        std::vector<std::string> GetListString(const std::string& param, const std::vector<std::string>& def_value = std::vector<std::string>());
    private:
        wxCmdLineParser& parser;
};

#endif
