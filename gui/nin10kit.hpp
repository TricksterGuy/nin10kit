#ifndef NIN10KIT_HPP
#define NIN10KIT_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "nin10kitgui.h"
#include "imageutil.hpp"

class Nin10KitViewerFrame;

class Nin10KitFrame : public Nin10KitGUI
{
    public:
        Nin10KitFrame();
        ~Nin10KitFrame();
		void OnLoadImages(wxCommandEvent& event);
        void OnImageSelected(wxListEvent& event);
		void OnDeleteAllImages(wxCommandEvent& event);
		void OnUpdateCurrentImage(wxCommandEvent& event);
		void OnDeleteCurrentImage(wxCommandEvent& event);
		void OnExport(wxCommandEvent& event);
		void OnView(wxCommandEvent& event);
		void OnCloseView(wxCloseEvent& event);
		void OnEditor(wxCommandEvent& event);
    private:
        std::unique_ptr<wxImageList> imageList;
        std::vector<std::string> filenames;
        std::map<std::string, ImageInfo> images;
        Nin10KitViewerFrame* viewer;
};

#endif
