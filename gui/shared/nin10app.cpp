#include "nin10app.hpp"
#include <wx/config.h>
#include <wx/dialog.h>
#include <wx/filename.h>
#include <Magick++.h>

#include "cpercep.hpp"
#include "wxlogger.hpp"

int main(int argc, char** argv)
{
    logger.reset(new LoggerWx());
#ifdef _WIN32
    // InitializeMagick doesn't seem to play nice...
    // Plus *argv is just the application binary name not a full path.
    wxFileName magick = wxFileName::DirName(wxFileName::GetCwd());
    magick.SetFullName("magick");
    Magick::InitializeMagick(magick.GetFullPath().ToStdString().c_str());
    MagickCore::SetClientPath(magick.GetFullPath().ToStdString().c_str());
#else
    Magick::InitializeMagick(*argv);
#endif
    cpercep_init();

    wxEntryStart(argc, argv);

    if (!wxTheApp->CallOnInit())
        return EXIT_FAILURE;

    wxTheApp->OnRun();

    return EXIT_SUCCESS;
}

bool Nin10App::OnInit()
{
    logger->SetLogLevel(LogLevel::INFO);
    if (!wxApp::OnInit())
        return false;

    wxInitAllImageHandlers();
    wxBitmap::InitStandardHandlers();
    wxDialog::EnableLayoutAdaptation(true);
#ifndef _WIN32
    wxHandleFatalExceptions();
#endif

    frame = OnInitFrame();
    SetTopWindow(frame);
    frame->Centre();
    frame->Show();

    return true;
}

void Nin10App::OnFatalException()
{
    FatalLog("An Fatal exception occured\n"
             "Please send the log messages to brandon - bwhitehead0308[AT]gmail[DOI]com");
}

void Nin10App::OnUnhandledException()
{
    FatalLog("An Unhandled exception occured\n"
             "Please send the log messages to brandon - bwhitehead0308[AT]gmail[DOI]com");
}

int Nin10App::OnExit()
{
    return wxApp::OnExit();
}
