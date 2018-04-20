///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 23 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __COLOR_CHOOSER_GUI_H__
#define __COLOR_CHOOSER_GUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/slider.h>
#include <wx/clrpicker.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ColorChooserGUI
///////////////////////////////////////////////////////////////////////////////
class ColorChooserGUI : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel6;
		wxStaticText* m_staticText1;
		wxSlider* red_slider;
		wxStaticText* m_staticText2;
		wxSlider* green_slider;
		wxStaticText* m_staticText3;
		wxSlider* blue_slider;
		wxStaticText* m_staticText5;
		wxColourPickerCtrl* color_picker;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnScroll( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnColor( wxColourPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		ColorChooserGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Color Chooser"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 320,360 ), long style = wxCAPTION|wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~ColorChooserGUI();
	
};

#endif //__COLOR_CHOOSER_GUI_H__
