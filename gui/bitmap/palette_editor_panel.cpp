#include "palette_editor_panel.hpp"
#include <wx/dcclient.h>
#include "wxlogger.hpp"
#include "color_chooser_dialog.hpp"
#include "colorutil.hpp"

#define PALETTE_RECT_SIZE 16
#define PALETTE_COLUMNS 16
#define MAX_PALETTE 256

PaletteEditorPanel::PaletteEditorPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
        wxScrolledWindow(parent, id, pos, size, style, name)
{
	Connect(wxEVT_CHAR,          wxKeyEventHandler(PaletteEditorPanel::OnChar),          nullptr, this);
	Connect(wxEVT_LEFT_DCLICK,   wxMouseEventHandler(PaletteEditorPanel::OnLeftDClick),  nullptr, this);
	Connect(wxEVT_LEFT_DOWN,     wxMouseEventHandler(PaletteEditorPanel::OnLeftDown),    nullptr, this);
	Connect(wxEVT_MIDDLE_DOWN,   wxMouseEventHandler(PaletteEditorPanel::OnMiddleDown),  nullptr, this);
	Connect(wxEVT_RIGHT_DOWN,    wxMouseEventHandler(PaletteEditorPanel::OnRightDown),   nullptr, this);
}

PaletteEditorPanel::~PaletteEditorPanel()
{
	Disconnect(wxEVT_CHAR,        wxKeyEventHandler(PaletteEditorPanel::OnChar),         nullptr, this);
	Disconnect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(PaletteEditorPanel::OnLeftDClick), nullptr, this);
	Disconnect(wxEVT_LEFT_DOWN,   wxMouseEventHandler(PaletteEditorPanel::OnLeftDown),   nullptr, this);
	Disconnect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(PaletteEditorPanel::OnMiddleDown), nullptr, this);
	Disconnect(wxEVT_RIGHT_DOWN,  wxMouseEventHandler(PaletteEditorPanel::OnRightDown),  nullptr, this);
}

int PaletteEditorPanel::GetSelectedIndex(const wxPoint& point) const
{
    wxPoint pos = CalcUnscrolledPosition(point);
    if (pos.x < 0 || pos.y < 0 || pos.x >= PALETTE_RECT_SIZE * PALETTE_COLUMNS || pos.y >= PALETTE_RECT_SIZE * PALETTE_COLUMNS)
        return -1;

    return pos.x / PALETTE_RECT_SIZE + pos.y / PALETTE_RECT_SIZE * PALETTE_COLUMNS;
}

Color16 PaletteEditorPanel::GetColor(int index, const Color16& color)
{
    Color16 c = color;
    if (static_cast<unsigned int>(index) < palette->Size())
        c = palette->At(index);
    return c;
}

void PaletteEditorPanel::OnChar(wxKeyEvent& event)
{

}

void PaletteEditorPanel::OnLeftDClick(wxMouseEvent& event)
{
    EventLog l(__func__);

    int index = GetSelectedIndex(event.GetPosition());
    Color16 c = GetColor(index);
    std::unique_ptr<ColorChooserDialog> ccdialog(new ColorChooserDialog(this, c));

    if (ccdialog->ShowModal() != wxID_OK)
    {
        VerboseLog("No color chosen");
        return;
    }

    Color16 color = ccdialog->GetColor();
    VerboseLog("Palette Index: %d Color: (%d %d %d)", index, color.r, color.g, color.b);

    if (!palette->Set(index, color))
        WarnLog("Failed to set palette index: %d to (%d %d %d)", index, color.r, color.g, color.b);

    UpdateColors();
    Refresh();
}

void PaletteEditorPanel::OnLeftDown(wxMouseEvent& event)
{
    EventLog l(__func__);
    lselection = GetSelectedIndex(event.GetPosition());
    VerboseLog("Left Selection: %d", lselection);
    UpdateColors();
    Refresh();
}

void PaletteEditorPanel::OnMiddleDown(wxMouseEvent& event)
{
    EventLog l(__func__);
    mselection = GetSelectedIndex(event.GetPosition());
    VerboseLog("Middle Selection: %d", mselection);
    UpdateColors();
    Refresh();
}

void PaletteEditorPanel::OnRightDown(wxMouseEvent& event)
{
    EventLog l(__func__);
    rselection = GetSelectedIndex(event.GetPosition());
    VerboseLog("Right Selection: %d", rselection);
    UpdateColors();
    Refresh();
}

void PaletteEditorPanel::UpdateColors()
{
    if (color_info_panel == nullptr)
        return;

    color_info_panel->SetColors(GetColor(lselection), GetColor(mselection), GetColor(rselection));
    graphics_notebook->Refresh();
    color_info_panel->Refresh();
}

void PaletteEditorPanel::OnDraw(wxDC& dc)
{
    if (palette == nullptr)
        return;

    for (unsigned int i = 0; i < MAX_PALETTE; i++)
    {
        int x = i % PALETTE_COLUMNS;
        int y = i / PALETTE_COLUMNS;
        SetFill(dc, GetColor(i));
        dc.DrawRectangle(x * PALETTE_RECT_SIZE, y * PALETTE_RECT_SIZE, PALETTE_RECT_SIZE, PALETTE_RECT_SIZE);
    }
}
