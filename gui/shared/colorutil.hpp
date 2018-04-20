#ifndef COLOR_UTIL_HPP
#define COLOR_UTIL_HPP

#include <wx/dcclient.h>
#include "color.hpp"

wxColor ToWxColor(const Color16& color);
void SetColor(wxDC& dc, const Color16& color);
void SetHighlight(wxDC& dc, const Color16& color);
void SetFill(wxDC& dc, const Color16& fill, const Color16& outline = Color16(0, 0, 0));

#endif
