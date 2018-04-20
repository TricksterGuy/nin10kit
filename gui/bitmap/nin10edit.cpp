#include "nin10edit.hpp"
#include "graphics_editor_panel.hpp"
#include "palette_editor_panel.hpp"
#include <wx/filedlg.h>
#include "wxlogger.hpp"

Nin10Edit::Nin10Edit() : Nin10EditGUI(nullptr)
{
    palette_panel->SetColorInfoPanel(color_info);
    palette_panel->SetGraphicsNotebook(graphics_notebook);

    wxCommandEvent dummy;
    OnNew(dummy);
}

void Nin10Edit::OnNew(wxCommandEvent& event)
{
}

void Nin10Edit::OnOpen(wxCommandEvent& event)
{
    EventLog l(__func__);
    std::unique_ptr<wxFileDialog> filedlg(new wxFileDialog(this, _("Open Image"), "", "", wxFileSelectorDefaultWildcardStr,
                                                           wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR | wxFD_PREVIEW | wxFD_OPEN));
    if (filedlg->ShowModal() == wxID_CANCEL)
    {
        WarnLog("No image given");
        return;
    }


    std::string file = filedlg->GetPath().ToStdString();
    InfoLog("Got file: %s", file.c_str());
    images.emplace(file, file);
    ConvertToMode4(images, images8);

    auto* graphics_panel = new GraphicsEditorPanel(graphics_notebook);
	graphics_notebook->AddPage(graphics_panel, file, false);

    graphics_panel->SetImage(&images8[0]);
    palette_panel->SetPalette(images8[0].palette);
}
