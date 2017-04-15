#include "cmd-line-parser-helper.hpp"
#include "scanner.hpp"
#include "logger.hpp"
#include <sstream>
#include <algorithm>

bool CmdLineParserHelper::GetSwitch(const std::string& param, bool def_value)
{
    std::string no_param = "no_" + param;
    auto found_switch = parser.FoundSwitch(param);
    auto no_found_switch = parser.FoundSwitch(no_param);
    if (found_switch != wxCMD_SWITCH_NOT_FOUND && no_found_switch != wxCMD_SWITCH_NOT_FOUND)
        FatalLog("-%s and -no_%s provided at the same time, only one of these switches can be given", param.c_str(), param.c_str());

    if (found_switch == wxCMD_SWITCH_NOT_FOUND && no_found_switch == wxCMD_SWITCH_NOT_FOUND) return def_value;

    return found_switch == wxCMD_SWITCH_ON || no_found_switch == wxCMD_SWITCH_OFF;
}

bool CmdLineParserHelper::GetBoolean(const std::string& param, bool def_value)
{
    long ret;
    if (!parser.Found(param, &ret)) return def_value;
    return ret != 0;
}

int CmdLineParserHelper::GetInt(const std::string& param, int def_value, int min, int max)
{
    long ret;
    if (!parser.Found(param, &ret)) return def_value;

    int val = ret;
    if (val < min || val > max)
    {
        WarnLog("Invalid value given for %s range [%d %d] given %d ignoring", param.c_str(), min, max, val);
        val = def_value;
    }
    return val;
}

int CmdLineParserHelper::GetHexInt(const std::string& param, int def_value, int min, int max)
{
    wxString hex;
    if (!parser.Found(param, &hex)) return def_value;
    long ret;
    if (!hex.ToLong(&ret, 16))
    {
        WarnLog("Could not parse %s into hex ignoring", param.c_str());
        return def_value;
    }

    int val = ret;
    if (val < min || val > max)
    {
        WarnLog("Invalid value given for %s range [%d %d] given %d ignoring", param.c_str(), min, max, val);
        val = def_value;
    }
    return val;
}


long CmdLineParserHelper::GetLong(const std::string& param, long def_value, long min, long max)
{
    long ret = std::max(std::min(def_value, max), min);
    parser.Found(param, &ret);
    return ret;
}

std::string CmdLineParserHelper::GetString(const std::string& param, const std::string& def_value)
{
    wxString ret;
    return parser.Found(param, &ret) ? ret.ToStdString() : def_value;
}

std::string CmdLineParserHelper::GetChoice(const std::string& param, const std::set<std::string>& choices, const std::string& def_value)
{
    std::string ret = GetString(param, def_value);
    if (choices.find(ret) == choices.end())
    {
        std::stringstream choices_str;
        unsigned int i = 0;
        for (const auto& choice : choices)
        {
            choices_str << choice;
            if (i != choices.size() - 1)
                choices_str << ",";
            i++;
        }
        FatalLog("Invalid choice given for %s.  Valid choices are [%s].", param.c_str(), choices_str.str().c_str());
    }
    return ret;
}

std::vector<int> CmdLineParserHelper::GetListInt(const std::string& param, const std::vector<int>& def_value)
{
    wxString list;
    if (!parser.Found(param, &list)) return def_value;

    std::vector<int> ret;
    Scanner scan(list.ToStdString(), ",");
    while (scan.HasMoreTokens())
    {
        int var;
        if (!scan.Next(var))
            FatalLog("Error parsing param %s", param.c_str());
        ret.push_back(var);
    }

    return ret;
}

std::vector<std::string> CmdLineParserHelper::GetListString(const std::string& param, const std::vector<std::string>& def_value)
{
    wxString list;
    if (!parser.Found(param, &list)) return def_value;

    std::vector<std::string> ret;
    Scanner scan(list.ToStdString(), ",");
    while (scan.HasMoreTokens())
    {
        std::string var;
        if (!scan.Next(var))
            FatalLog("Error parsing param %s", param.c_str());
        ret.push_back(var);
    }

    return ret;
}
