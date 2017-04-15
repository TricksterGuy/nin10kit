#ifndef NIN10KITAPP_HPP
#define NIN10KITAPP_HPP

#include <wx/app.h>
#include "nin10kit.hpp"

class Nin10KitApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        void OnFatalException();
        void OnUnhandledException();
        Nin10KitFrame* GetFrame() {return frame;}
    private:
        Nin10KitFrame* frame = nullptr;
};

DECLARE_APP(Nin10KitApp);

#endif

