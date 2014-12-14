#include "scanner.hpp"
#include "logger.hpp"

bool Scanner::Next(int& var, int base)
{
    if (!HasMoreTokens()) return false;

    wxString token = GetNextToken();
    VerboseLog("Read int %s base %d", static_cast<const char*>(token.c_str()), base);
    long ret;
    if (!token.ToLong(&ret, base)) return false;

    var = ret;
    return true;
}

bool Scanner::Next(unsigned int& var, int base)
{
    if (!HasMoreTokens()) return false;

    wxString token = GetNextToken();
    VerboseLog("Read unsigned int %s base %d", static_cast<const char*>(token.c_str()), base);
    unsigned long ret;
    if (!token.ToULong(&ret, base)) return false;

    var = ret;
    return true;
}

bool Scanner::Next(float& var)
{
    if (!HasMoreTokens()) return false;

    wxString token = GetNextToken();
    VerboseLog("Read float %s", static_cast<const char*>(token.c_str()));
    double ret;
    if (!token.ToDouble(&ret)) return false;

    var = (float) ret;

    return true;
}

bool Scanner::Next(double& var)
{
    if (!HasMoreTokens()) return false;

    wxString token = GetNextToken();
    VerboseLog("Read double %s", static_cast<const char*>(token.c_str()));

    if (!token.ToDouble(&var)) return false;

    return true;
}

bool Scanner::Next(std::string& var)
{
    if (!HasMoreTokens()) return false;

    var = GetNextToken();
    VerboseLog("Read string %s", static_cast<const char*>(var.c_str()));
    return true;
}

bool Scanner::NextLine(std::string& var)
{
    if (!HasMoreTokens()) return false;

    var = GetString();
    VerboseLog("Read line %s", static_cast<const char*>(var.c_str()));
    return true;
}

