#include "graphics_editor_panel.hpp"
#include <wx/dcclient.h>
#include "colorutil.hpp"

void DrawImage8(wxDC& dc, Image8Bpp* image)
{
    for (unsigned int x = 0; x < image->width; x++)
    {
        for (unsigned int y = 0; y < image->height; y++)
        {
            unsigned char index = image->At(x, y);
            Color16 c = image->palette->At(index);
            if (index == 0)
            {
                int ii = y * image->width + x;
                c = ((ii >> 2 & 1) ^ (y >> 2 & 1)) ? Color16(31, 31, 31) : Color16(24, 24, 24);
            }

            SetColor(dc, c);
            dc.DrawPoint(x, y);
        }
    }
}

void DrawImage16(wxDC& dc, Image16Bpp* image)
{
    for (unsigned int x = 0; x < image->width; x++)
    {
        for (unsigned int y = 0; y < image->height; y++)
        {
            SetColor(dc, image->At(x, y));
            dc.DrawPoint(x, y);
        }
    }
}

void GraphicsEditorPanel::OnDraw(wxDC& dc)
{
    if (image == nullptr)
        return;

    auto* image8 = dynamic_cast<Image8Bpp*>(image);
    auto* image16 = dynamic_cast<Image16Bpp*>(image);

    if (image8 != nullptr)
        DrawImage8(dc, image8);
    else if (image16 != nullptr)
        DrawImage16(dc, image16);
}
