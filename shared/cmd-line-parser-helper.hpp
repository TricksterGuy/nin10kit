#ifndef CMD_LINE_PARSER_HELPER_HPP
#define CMD_LINE_PARSER_HELPER_HPP

#include <limits>
#include <set>
#include <string>
#include <vector>

#include <wx/cmdline.h>

/** Wrapper around wxWidget's command line parser library */
class CmdLineParserHelper
{
    public:
        CmdLineParserHelper(wxCmdLineParser& parser_) : parser(parser_) {}
        /** Get value of boolean switch */
        bool GetSwitch(const std::string& param, bool def_value = false);
        /** Get value of boolean passed into param */
        bool GetBoolean(const std::string& param, bool def_value = false);
        /** Get value of integer passed into param */
        int GetInt(const std::string& param, int def_value = 0, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max());
        /** Get value of integer passed as hex into param */
        int GetHexInt(const std::string& param, int def_value = 0, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max());
        /** Get value of long passed into param */
        long GetLong(const std::string& param, long def_value = 0, long min = std::numeric_limits<long>::min(), long max = std::numeric_limits<long>::max());
        /** Get value of string passed into param */
        std::string GetString(const std::string& param, const std::string& def_value = "");
        /** Get value of choice passed into param */
        std::string GetChoice(const std::string& param, const std::set<std::string>& choices, const std::string& def_value = "");
        /** Get comma separated list of ints passed into param */
        std::vector<int> GetListInt(const std::string& param, const std::vector<int>& def_value = std::vector<int>());
        /** Get comma separated list of strings passed into param */
        std::vector<std::string> GetListString(const std::string& param, const std::vector<std::string>& def_value = std::vector<std::string>());
    private:
        wxCmdLineParser& parser;
};

#endif
