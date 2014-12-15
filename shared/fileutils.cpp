#include "fileutils.hpp"

#include <cstdio>
#include <cstdlib>

#include "shared.hpp"

void InitFiles(std::ofstream& file_c, std::ofstream& file_h, const std::string& name)
{
    VerboseLog("Init Files");
    std::string filename_c = name + ".c";
    std::string filename_h = name + ".h";

    file_c.open(filename_c.c_str());
    file_h.open(filename_h.c_str());

    if (!file_c.good() || !file_h.good())
        FatalLog("Could not open files for writing");
}

void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append, const std::vector<unsigned short>& data, unsigned int items_per_row)
{
    VerboseLog("Writing short array %s%s size %zd", name.c_str(), append.c_str(), data.size());
    char buffer[7];
    file << "const unsigned short " << name << append << "[" << data.size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < data.size(); i++)
    {
        snprintf(buffer, 7, "0x%04x", data[i]);
        WriteElement(file, buffer, data.size(), i, items_per_row);
    }
    file << "\n};\n";
}

void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append, const std::vector<unsigned char>& data, unsigned int items_per_row)
{
    VerboseLog("Writing short array (from char) %s%s size %zd", name.c_str(), append.c_str(), data.size() / 2);
    char buffer[7];
    unsigned int size = data.size() / 2;
    file << "const unsigned short " << name << append << "[" << size << "] =\n{\n\t";
    for (unsigned int i = 0; i < size; i++)
    {
        snprintf(buffer, 7, "0x%02x%02x", data[2 * i + 1], data[2 * i]);
        WriteElement(file, buffer, size, i, items_per_row);
    }
    file << "\n};\n";
}

void WriteShortArray4Bit(std::ostream& file, const std::string& name, const std::string& append, const std::vector<unsigned char>& data, unsigned int items_per_row)
{
    VerboseLog("Writing short array (from 4 bits) %s%s size %zd", name.c_str(), append.c_str(), data.size());
    char buffer[7];
    unsigned int size = data.size() / 4;
    file << "const unsigned short " << name << append << "[" << size << "] =\n{\n\t";
    for (unsigned int i = 0; i < size; i++)
    {
        snprintf(buffer, 7, "0x%01x%01x%01x%01x", data[4 * i + 3], data[4 * i + 2], data[4 * i + 1], data[4 * i]);
        WriteElement(file, buffer, size, i, items_per_row);
    }
    file << "\n};\n";
}

void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append, const std::vector<Color>& data, unsigned int items_per_row)
{
    VerboseLog("Writing short array (from Colors) %s%s size %zd", name.c_str(), append.c_str(), data.size());
    char buffer[7];
    file << "const unsigned short " << name << append << "[" << data.size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < data.size(); i++)
    {
        unsigned short data_read = data[i].GetBGR15();
        snprintf(buffer, 7, "0x%04x", data_read);
        WriteElement(file, buffer, data.size(), i, items_per_row);
    }
    file << "\n};\n";
}

void WriteCharArray(std::ostream& file, const std::string& name, const std::string& append, const std::vector<unsigned char>& data, unsigned int items_per_row)
{
    VerboseLog("Writing char array %s%s size %zd", name.c_str(), append.c_str(), data.size());
    char buffer[5];
    file << "const unsigned char " << name << append << "[" << data.size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < data.size(); i++)
    {
        snprintf(buffer, 5, "0x%02x", data[i]);
        WriteElement(file, buffer, data.size(), i, items_per_row);
    }
    file << "\n};\n";
}


void WriteElement(std::ostream& file, const std::string& data, unsigned int size, unsigned int counter,
                  unsigned int items_per_row)
{
    file << data;
    if (counter != size - 1)
    {
        file << ",";
        if (counter % items_per_row == items_per_row - 1)
            file << "\n\t";
    }
}

void WriteAnimationArray(std::ostream& file, const std::string& type, const std::string& name,
                         const std::string& append, const std::vector<std::string>& ptr_names,
                         unsigned int items_per_row)
{
    VerboseLog("Writing Animation %s array %s%s size %zd", type.c_str(), name.c_str(), append.c_str(), ptr_names.size());
    file << type << " " << name << append << "[" << ptr_names.size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < ptr_names.size(); i++)
    {
        WriteElement(file, ptr_names[i], ptr_names.size(), i, items_per_row);
    }
    file << "\n};\n";
}

void WriteExtern(std::ostream& file, const std::string& type, const std::string& name, const std::string& append, unsigned int size)
{
    VerboseLog("Writing extern %s %s%s size %zd", type.c_str(), name.c_str(), append.c_str(), size);
    file << "extern " << type << " " << name << append << "[" << size << "];\n";
}

void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, int value)
{
    std::string name_cap = ToUpper(name);
    VerboseLog("Writing define %s%s value %d", name.c_str(), append.c_str(), value);
    file << "#define " << name_cap << append << " " << value << "\n";
}

void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, int value, int shift)
{
    std::string name_cap = ToUpper(name);
    VerboseLog("Writing define %s%s value %d<<%d", name.c_str(), append.c_str(), value, shift);
    file << "#define " << name_cap << append << " (" << value << " << " << shift << ")\n";
}

void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, const std::string& value)
{
    std::string name_cap = ToUpper(name);
    VerboseLog("Writing define %s%s value %s", name_cap.c_str(), append.c_str(), value.c_str());
    file << "#define " << name_cap << append << " " << value << "\n";
}

void WriteHeaderGuard(std::ostream& file, const std::string& name, const std::string& append)
{
    std::string name_cap = ToUpper(name);
    VerboseLog("Writing header guard %s%s", name_cap.c_str(), append.c_str());
    file << "#ifndef " << name_cap << append << "\n";
    file << "#define " << name_cap << append << "\n\n";
}

void WriteInclude(std::ostream& file, const std::string& filename)
{
    file << "#include \"" << filename << "\"\n";
}

void WriteEndHeaderGuard(std::ostream& file)
{
    VerboseLog("Writing end of header guard");
    file << "#endif\n";
}

void WriteNewLine(std::ostream& file)
{
    file << "\n";
}
