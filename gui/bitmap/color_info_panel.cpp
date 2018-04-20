#include "color_info_panel.hpp"
#include "wxlogger.hpp"
#include "colorutil.hpp"

bool PointInCircle(const wxPoint& point, const wxPoint& center, int radius)
{
    int x_diff = point.x - center.x;
    int y_diff = point.y - center.y;
    return x_diff * x_diff + y_diff * y_diff <= radius * radius;
}

ColorInfoPanel::ColorInfoPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
    wxScrolledWindow(parent, id, pos, size, style, name)
{
    Connect(wxEVT_LEFT_DOWN,    wxMouseEventHandler(ColorInfoPanel::OnLeftDown), nullptr, this);
}

ColorInfoPanel::~ColorInfoPanel()
{
	Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(ColorInfoPanel::OnLeftDown), nullptr, this);
}

void ColorInfoPanel::OnLeftDown(wxMouseEvent& event)
{
    EventLog l(__func__);
    wxPoint pos = CalcUnscrolledPosition(event.GetPosition());

    if (PointInCircle(pos, wxPoint(96, 64), 32))
        VerboseLog("left");
    else if (PointInCircle(pos, wxPoint(160, 64), 32))
        VerboseLog("right");
    else if (PointInCircle(pos, wxPoint(128, 32), 32))
        VerboseLog("middle");
}

void ColorInfoPanel::OnDraw(wxDC& dc)
{
    wxSize size = GetClientSize();
    SetFill(dc, middle);
    dc.DrawCircle(128, 32, 32);
    SetHighlight(dc, middle);
    dc.DrawLabel("M", wxRect(96, -4, 64, 64), wxALIGN_CENTER);
    SetFill(dc, left);
    dc.DrawCircle(96, 64, 32);
    SetHighlight(dc, left);
    dc.DrawLabel("L", wxRect(64, 32, 64, 64), wxALIGN_CENTER);
    SetFill(dc, right);
    dc.DrawCircle(160, 64, 32);
    SetHighlight(dc, right);
    dc.DrawLabel("R", wxRect(128, 32, 64, 64), wxALIGN_CENTER);
}
