#ifndef COLOR_CHOOSER_DIALOG_HPP
#define COLOR_CHOOSER_DIALOG_HPP

#include "color_chooser_gui.h"
#include "color.hpp"

class ColorChooserDialog : public ColorChooserGUI
{
public:
    ColorChooserDialog(wxWindow* parent, const Color16& color);
    Color16 GetColor();
    void OnScroll(wxScrollEvent& event) override;
    void OnColor(wxColourPickerEvent& event) override;
};

#endif
