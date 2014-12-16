#include "nin10kitapp.hpp"
#include <wx/config.h>
#include <wx/dialog.h>

IMPLEMENT_APP(Nin10KitApp);

bool Nin10KitApp::OnInit()
{
    wxInitAllImageHandlers();
    wxDialog::EnableLayoutAdaptation(true);

    // Fill in the application information fields before creating wxConfig.
    SetVendorName("wxWidgets");
    SetAppName("wx_docview_sample");
    SetAppDisplayName("wxWidgets DocView Sample");

    frame = new Nin10KitFrame(NULL);
    SetTopWindow(frame);
    frame->Centre();
    frame->Show();

    //frame->UpdateStatusBar();

    return true;
}

int Nin10KitApp::OnExit()
{
    return wxApp::OnExit();
}
