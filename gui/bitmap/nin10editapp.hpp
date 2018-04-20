#ifndef NIN10KITAPP_HPP
#define NIN10KITAPP_HPP

#include "nin10app.hpp"
#include "nin10edit.hpp"
#include "wxlogger.hpp"

class Nin10EditApp : public Nin10App
{
    public:
        wxFrame* OnInitFrame() override
        {
            return new Nin10Edit();
        }
        void OnInitCmdLine(wxCmdLineParser& parser) override;
        bool OnCmdLineParsed(wxCmdLineParser& parser) override;
};

DECLARE_APP(Nin10EditApp);

#endif

