#ifndef NIN10APP_HPP
#define NIN10APP_HPP

#include <wx/app.h>
#include <wx/frame.h>

class Nin10App : public wxApp
{
    public:
        virtual bool OnInit();
        virtual wxFrame* OnInitFrame() = 0;
        virtual int  OnExit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser) = 0;
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser) = 0;
        void OnFatalException();
        void OnUnhandledException();
        wxFrame* GetFrame() const {return frame;}
    private:
        wxFrame* frame;
};

#endif
