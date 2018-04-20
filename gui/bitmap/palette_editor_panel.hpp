#ifndef PALETTE_EDITOR_PANEL_HPP
#define PALETTE_EDITOR_PANEL_HPP

#include <wx/scrolwin.h>
#include <wx/notebook.h>
#include "color_info_panel.hpp"
#include "graphics_editor_panel.hpp"
#include "alltypes.hpp"

class PaletteEditorPanel : public wxScrolledWindow
{
public:
    PaletteEditorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                       long style = wxTAB_TRAVERSAL, const wxString& name = wxPanelNameStr);
    ~PaletteEditorPanel();
    void SetPalette(const std::shared_ptr<Palette>& _palette)
    {
        palette = _palette;
        Refresh();
    }

    int GetSelectedIndex(const wxPoint& point) const;
    Color16 GetColor(int index, const Color16& color = Color16(0, 0, 0));

    void OnChar(wxKeyEvent& event);
    void OnLeftDClick(wxMouseEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnMiddleDown(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnDraw(wxDC& event);

    void UpdateColors();

    void SetColorInfoPanel(ColorInfoPanel* panel) {color_info_panel = panel;}
    void SetGraphicsNotebook(wxNotebook* panel) {graphics_notebook = panel;}
private:
    std::shared_ptr<Palette> palette;
    int lselection = -1;
    int mselection = -1;
    int rselection = -1;
    ColorInfoPanel* color_info_panel = nullptr;
    wxNotebook* graphics_notebook = nullptr;
};

#endif
