#ifndef NIN10KITAPP_HPP
#define NIN10KITAPP_HPP

#include <wx/app.h>
#include "nin10kit.h"

class Nin10KitApp : public wxApp
{
    public:
        bool OnInit();
        int OnExit();
        Nin10KitFrame* GetFrame() {return frame;}
    private:
        Nin10KitFrame* frame;
};

DECLARE_APP(Nin10KitApp);

#endif

