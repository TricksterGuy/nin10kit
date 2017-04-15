#include "shared.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <wx/filename.h>

std::string ToUpper(const std::string& str)
{
    std::string cap = str;
    transform(cap.begin(), cap.end(), cap.begin(), (int(*)(int)) std::toupper);
    return cap;
}

void split(const std::string& s, char delimiter, std::vector<std::string>& tokens)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delimiter))
        tokens.push_back(item);
}

std::string Chop(const std::string& filename)
{
    return wxFileName(filename).GetName().ToStdString();
}

std::string Sanitize(const std::string& filename)
{
    std::stringstream out;
    for (unsigned int i = 0; i < filename.size(); i++)
    {
        if ((filename[i] >= 'A' && filename[i] <= 'Z') ||
            (filename[i] >= 'a' && filename[i] <= 'z') ||
            (filename[i] >= '0' && filename[i] <= '9' && i != 0) ||
            filename[i] == '_')
            out.put(filename[i]);
    }
    return out.str();
}

std::string Format(const std::string& file)
{
    return Sanitize(Chop(file));
}

unsigned int log2(unsigned int x)
{
    unsigned int result = 0;
    while (x >>= 1) result++;
    return result;
}
