///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 23 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NIN10EDITGUI_H__
#define __NIN10EDITGUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
class ColorInfoPanel;
class PaletteEditorPanel;

#include <wx/gdicmn.h>
#include <wx/notebook.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define ID_NEW 1000
#define ID_OPEN 1001
#define ID_SAVE 1002
#define ID_SAVE_AS 1003
#define ID_QUIT 1004

///////////////////////////////////////////////////////////////////////////////
/// Class Nin10EditGUI
///////////////////////////////////////////////////////////////////////////////
class Nin10EditGUI : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_panel1;
		wxPanel* m_panel2;
		wxNotebook* graphics_notebook;
		wxPanel* m_panel3;
		wxStaticBoxSizer* palette_sizer;
		PaletteEditorPanel* palette_panel;
		ColorInfoPanel* color_info;
		wxMenuBar* menu_bar;
		wxMenu* file;
		wxMenu* edit;
		wxMenu* view;
		wxMenu* tools;
		wxMenu* help;
		wxStatusBar* status_bar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnNew( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSaveAs( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		Nin10EditGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxRESIZE_BORDER|wxTAB_TRAVERSAL );
		
		~Nin10EditGUI();
	
};

#endif //__NIN10EDITGUI_H__
