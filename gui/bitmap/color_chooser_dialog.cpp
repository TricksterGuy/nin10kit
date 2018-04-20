#include "color_chooser_dialog.hpp"
#include "wxlogger.hpp"
#include "colorutil.hpp"

ColorChooserDialog::ColorChooserDialog(wxWindow* parent, const Color16& color) : ColorChooserGUI(parent)
{
    red_slider->SetValue(color.r);
    green_slider->SetValue(color.g);
    blue_slider->SetValue(color.b);
    color_picker->SetColour(ToWxColor(color));
}

Color16 ColorChooserDialog::GetColor()
{
    return Color16(red_slider->GetValue(), green_slider->GetValue(), blue_slider->GetValue());
}

void ColorChooserDialog::OnScroll(wxScrollEvent& event)
{
    EventLog l(__func__);
    color_picker->SetColour(wxColor(red_slider->GetValue() << 3, green_slider->GetValue() << 3, blue_slider->GetValue() << 3));
}

void ColorChooserDialog::OnColor(wxColourPickerEvent& event)
{
    EventLog l(__func__);
    const auto& color = event.GetColour();
    red_slider->SetValue(color.Red() >> 3 & 0x1F);
    green_slider->SetValue(color.Green() >> 3 & 0x1F);
    blue_slider->SetValue(color.Blue() >> 3 & 0x1F);
    color_picker->SetColour(wxColor(red_slider->GetValue() << 3, green_slider->GetValue() << 3, blue_slider->GetValue() << 3));
}
