#include "colorutil.hpp"

wxColor ToWxColor(const Color16& color)
{
    return wxColor(color.r << 3, color.g << 3, color.b << 3);
}

void SetColor(wxDC& dc, const Color16& color)
{
    dc.SetPen(wxPen(ToWxColor(color)));
}

void SetHighlight(wxDC& dc, const Color16& color)
{
    wxColor c;
    if (color.r + color.g + color.b <= 45)
        c = *wxWHITE;
    else
        c = *wxBLACK;

    dc.SetTextForeground(c);
    //dc.SetPen(wxPen(c));
}

void SetFill(wxDC& dc, const Color16& fill, const Color16& outline)
{
    dc.SetBrush(wxBrush(ToWxColor(fill)));
    SetColor(dc, outline);
}
