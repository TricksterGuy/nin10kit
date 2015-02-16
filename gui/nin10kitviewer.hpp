#ifndef NIN10KIT_VIEWER_HPP
#define NIN10KIT_VIEWER_HPP

#include "nin10kitviewergui.h"
#include <map>
#include <vector>
#include <wx/bitmap.h>
#include "imageutil.hpp"

class Nin10KitViewerFrame : public Nin10KitViewerGUI
{
    public:
        Nin10KitViewerFrame();
        ~Nin10KitViewerFrame();
        bool Set(int mode, std::map<std::string, ImageInfo>& images);
        void OnPrev(wxCommandEvent& event);
		void OnNext(wxCommandEvent& event);
    private:
        void UpdateMode3(std::map<std::string, ImageInfo>& images);
        void UpdateMode4(std::map<std::string, ImageInfo>& images);
        void UpdateMode0(std::map<std::string, ImageInfo>& images, int bpp);
        void UpdateSprites(std::map<std::string, ImageInfo>& images, int bpp);
        void UpdateGraphicsWindow();
        std::vector<wxBitmap> graphics;
        wxBitmap tileset;
        wxBitmap palette;
        int selectedGraphic;
        int selectedPalette;
        int selectedTile;
};

#endif
