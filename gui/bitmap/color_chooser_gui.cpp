///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 23 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "color_chooser_gui.h"

///////////////////////////////////////////////////////////////////////////

ColorChooserGUI::ColorChooserGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_panel6 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panel6, wxID_ANY, wxT("Red"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	red_slider = new wxSlider( m_panel6, wxID_ANY, 0, 0, 31, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizer1->Add( red_slider, 1, wxEXPAND|wxALL, 5 );
	
	m_staticText2 = new wxStaticText( m_panel6, wxID_ANY, wxT("Green"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	green_slider = new wxSlider( m_panel6, wxID_ANY, 0, 0, 31, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizer1->Add( green_slider, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( m_panel6, wxID_ANY, wxT("Blue"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	blue_slider = new wxSlider( m_panel6, wxID_ANY, 0, 0, 31, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizer1->Add( blue_slider, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( m_panel6, wxID_ANY, wxT("Color"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	color_picker = new wxColourPickerCtrl( m_panel6, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL|wxCLRP_USE_TEXTCTRL );
	fgSizer1->Add( color_picker, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer9->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button1 = new wxButton( m_panel6, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button1, 1, wxALL|wxEXPAND, 5 );
	
	m_button2 = new wxButton( m_panel6, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button2, 1, wxALL|wxEXPAND|wxALIGN_BOTTOM, 5 );
	
	
	bSizer9->Add( bSizer8, 0, wxALIGN_RIGHT|wxALIGN_BOTTOM, 5 );
	
	
	m_panel6->SetSizer( bSizer9 );
	m_panel6->Layout();
	bSizer9->Fit( m_panel6 );
	bSizer6->Add( m_panel6, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer6 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	red_slider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	color_picker->Connect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( ColorChooserGUI::OnColor ), NULL, this );
}

ColorChooserGUI::~ColorChooserGUI()
{
	// Disconnect Events
	red_slider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	red_slider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	green_slider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	blue_slider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ColorChooserGUI::OnScroll ), NULL, this );
	color_picker->Disconnect( wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler( ColorChooserGUI::OnColor ), NULL, this );
	
}
