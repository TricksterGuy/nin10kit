#include "nin10kitapp.hpp"
#include "cmd-line-parser-helper.hpp"
#include "wxlogger.hpp"

IMPLEMENT_APP_NO_MAIN(Nin10KitApp);

static const wxCmdLineEntryDesc cmd_descriptions[] =
{
    {wxCMD_LINE_SWITCH, "h", "help", "Displays help on command line parameters",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},

    // Debugging
    {wxCMD_LINE_OPTION, "log", "log", "Debug logging -log=num (0:fatal,1:error,2:warn,3:info,4:verbose) default 3",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    {wxCMD_LINE_NONE}
};

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
