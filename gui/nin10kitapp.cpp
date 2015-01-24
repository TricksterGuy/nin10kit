#include "nin10kitapp.hpp"
#include <wx/config.h>
#include <wx/dialog.h>
#include <Magick++.h>

#include "cmd-line-parser-helper.hpp"
#include "cpercep.hpp"
#include "logger.hpp"

IMPLEMENT_APP_NO_MAIN(Nin10KitApp);

int main(int argc, char** argv)
{
    Magick::InitializeMagick(*argv);
    cpercep_init();

    wxEntryStart(argc, argv);

    if (!wxTheApp->CallOnInit())
        return EXIT_FAILURE;

    wxTheApp->OnRun();

    return EXIT_SUCCESS;
}

static const wxCmdLineEntryDesc cmd_descriptions[] =
{
    {wxCMD_LINE_SWITCH, "h", "help", "Displays help on command line parameters",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},

    // Debugging
    {wxCMD_LINE_OPTION, "log", "log", "Debug logging -log=num (0:fatal,1:error,2:warn,3:info,4:verbose) default 3",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_NONE}
};

bool Nin10KitApp::OnInit()
{
    logger->SetLogLevel(LogLevel::INFO);
    if (!wxApp::OnInit())
        FatalLog("A problem occurred");

    wxInitAllImageHandlers();
    wxDialog::EnableLayoutAdaptation(true);
#ifndef _WIN32
    //wxHandleFatalExceptions();
#endif

    frame = new Nin10KitFrame();
    SetTopWindow(frame);
    frame->Centre();
    frame->Show();

    return true;
}

/** @brief OnInitCmdLine
  *
  * @todo: document this function
  */
void Nin10KitApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetLogo(wxString::Format(_("Nin10kit GUI")));
    parser.SetDesc(cmd_descriptions);
    parser.SetSwitchChars (_("-"));
}

bool Nin10KitApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    CmdLineParserHelper parse(parser);
    logger->SetLogLevel((LogLevel)parse.GetInt("log", 3, 0, 4));
    return true;
}

//void Nin10KitApp::OnFatalException()
//{
//    wxPuts("LOL");
//}

void Nin10KitApp::OnUnhandledException()
{
    wxPuts("LOL2");
}

int Nin10KitApp::OnExit()
{
    return wxApp::OnExit();
}

