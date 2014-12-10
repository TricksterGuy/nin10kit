#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "color.hpp"

void InitFiles(std::ofstream& c_file, std::ofstream& h_file, const std::string& name);
void WriteElement(std::ostream& file, const std::string& data, unsigned int size, unsigned int counter,
                  unsigned int items_per_row);

/// TODO rewrite this.
void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append,
                     const std::vector<unsigned short>& data, unsigned int items_per_row);
void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append,
                     const std::vector<unsigned char>& data, unsigned int items_per_row);
void WriteShortArray(std::ostream& file, const std::string& name, const std::string& append,
                     const std::vector<Color>& data, unsigned int items_per_row);
void WriteShortArray4Bit(std::ostream& file, const std::string& name, const std::string& append,
                     const std::vector<unsigned char>& data, unsigned int items_per_row);
void WriteAnimationArray(std::ostream& file, const std::string& type, const std::string& name,
                         const std::string& append, const std::vector<std::string>& ptr_names,
                         unsigned int items_per_row);


void WriteCharArray(std::ostream& file, const std::string& name, const std::string& append,
                    const std::vector<unsigned char>& data, unsigned int items_per_row);

void WriteExtern(std::ostream& file, const std::string& type, const std::string& name, const std::string& append, unsigned int size);
void WriteHeaderGuard(std::ostream& file, const std::string& name, const std::string& append);
void WriteEndHeaderGuard(std::ostream& file);
void WriteNewLine(std::ostream& file);
void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, int value);
void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, int value, int shift);
void WriteDefine(std::ostream& file, const std::string& name, const std::string& append, const std::string& value);
void WriteInclude(std::ostream& file, const std::string& filename);

#endif
