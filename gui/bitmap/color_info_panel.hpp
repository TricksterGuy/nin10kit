#ifndef COLOR_INFO_PANEL_HPP
#define COLOR_INFO_PANEL_HPP

#include <wx/scrolwin.h>
#include <wx/dcclient.h>
#include "color.hpp"

class ColorInfoPanel : public wxScrolledWindow
{
public:
    ColorInfoPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                       long style = wxTAB_TRAVERSAL, const wxString& name = wxPanelNameStr);
    ~ColorInfoPanel();
    void SetColors(const Color16& _left, const Color16& _middle, const Color16& _right)
    {
        left = _left;
        middle = _middle;
        right = _right;
        Refresh();
    }
    void OnLeftDown(wxMouseEvent& event);
    void OnDraw(wxDC& dc);
private:
    Color16 left;
    Color16 middle;
    Color16 right;
};

#endif
