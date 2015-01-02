#ifndef __NIN10KITGUI_H__
#define __NIN10KITGUI_H__

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/splitter.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class Nin10KitGUI
///////////////////////////////////////////////////////////////////////////////
class Nin10KitGUI : public wxFrame
{
	private:

	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxListCtrl* imagesList;
		wxPanel* m_panel2;
		wxStaticText* m_staticText7;
		wxChoice* mode;
		wxButton* m_button2;
		wxButton* m_button3;
		wxStaticText* m_staticText1;
		wxTextCtrl* imageFilename;
		wxStaticText* m_staticText3;
		wxTextCtrl* imageName;
		wxStaticText* m_staticText5;
		wxSpinCtrl* imageWidth;
		wxStaticText* m_staticText6;
		wxSpinCtrl* imageHeight;
		wxStaticText* m_staticText4;
		wxCheckBox* imageAnimated;
		wxButton* m_button6;
		wxButton* m_button1;
		wxButton* m_button4;
		wxButton* m_button5;

		// Virtual event handlers, overide them in your derived class
		virtual void OnImageSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnLoadImages( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteAllImages( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateCurrentImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteCurrentImage( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditor( wxCommandEvent& event ) { event.Skip(); }


	public:

		Nin10KitGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Nin10Kit"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~Nin10KitGUI();

		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 320 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( Nin10KitGUI::m_splitter1OnIdle ), NULL, this );
		}

};

#endif
