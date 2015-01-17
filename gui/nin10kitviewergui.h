///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NIN10KITVIEWERGUI_H__
#define __NIN10KITVIEWERGUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class Nin10KitViewerGUI
///////////////////////////////////////////////////////////////////////////////
class Nin10KitViewerGUI : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_panel3;
		wxBoxSizer* sharedSizer;
		wxStaticBoxSizer* tilesetSizer;
		wxScrolledWindow* m_scrolledWindow3;
		wxStaticBitmap* tilesetBitmap;
		wxStaticBoxSizer* paletteSizer;
		wxScrolledWindow* m_scrolledWindow2;
		wxStaticBitmap* paletteBitmap;
		wxScrolledWindow* m_scrolledWindow1;
		wxStaticBitmap* graphicsBitmap;
		wxButton* m_button8;
		wxButton* m_button9;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnPrev( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNext( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		Nin10KitViewerGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Viewer"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~Nin10KitViewerGUI();
	
};

#endif //__NIN10KITVIEWERGUI_H__
