#ifndef GRAPHICS_EDITOR_PANEL_HPP
#define GRAPHICS_EDITOR_PANEL_HPP

#include <memory>
#include <wx/dc.h>
#include <wx/scrolwin.h>
#include "alltypes.hpp"

class GraphicsEditorPanel : public wxScrolledWindow
{
public:
    GraphicsEditorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                       long style = wxTAB_TRAVERSAL, const wxString& name = wxPanelNameStr) : wxScrolledWindow(parent, id, pos, size, style, name) {}
    void SetImage(Image* _image)
    {
        image = _image;
        Refresh();
    }
    void OnDraw(wxDC& event);
private:
    Image* image;
};

#endif
