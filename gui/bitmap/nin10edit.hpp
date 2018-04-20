#ifndef NIN10EDIT_HPP
#define NIN10EDIT_HPP

#include "nin10editgui.h"
#include "graphics_editor_panel.hpp"
#include "imageutil.hpp"
#include "alltypes.hpp"

class Nin10Edit : public Nin10EditGUI
{
public:
    Nin10Edit();
    void OnNew(wxCommandEvent& event) override;
    void OnOpen(wxCommandEvent& event) override;
private:
    std::map<std::string, EditImageInfo> images;
    std::map<std::string, GraphicsEditorPanel*> pages;
};

#endif
