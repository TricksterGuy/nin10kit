#ifndef SHARED_HPP
#define SHARED_HPP

#include <set>
#include <string>
#include <vector>

std::string ToUpper(const std::string& str);
std::string Chop(const std::string& filename);
std::string Sanitize(const std::string& filename);
std::string Format(const std::string& filename);
unsigned int log2(unsigned int x);

extern const std::set<std::string> valid_3ds_modes;
extern const std::set<std::string> valid_gba_modes;

#endif
