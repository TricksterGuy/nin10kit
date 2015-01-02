#include "image.hpp"

#include <sstream>

Image::Image(unsigned int _width, unsigned int _height, const std::string& _name, const std::string& _filename, unsigned int _frame, bool _animated) :
    Exportable(_name), width(_width), height(_height), filename(_filename), frame(_frame), animated(_animated)
{
    if (animated)
    {
        std::stringstream oss;
        oss << name << frame;
        export_name = oss.str();
    }
    else
        export_name = name;
}
