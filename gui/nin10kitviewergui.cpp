///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "nin10kitviewergui.h"

///////////////////////////////////////////////////////////////////////////

Nin10KitViewerGUI::Nin10KitViewerGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_panel3 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	sharedSizer = new wxBoxSizer( wxVERTICAL );
	
	tilesetSizer = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, _("Tileset") ), wxVERTICAL );
	
	m_scrolledWindow3 = new wxScrolledWindow( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow3->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	tilesetBitmap = new wxStaticBitmap( m_scrolledWindow3, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( tilesetBitmap, 1, wxALL|wxEXPAND, 0 );
	
	
	m_scrolledWindow3->SetSizer( bSizer15 );
	m_scrolledWindow3->Layout();
	bSizer15->Fit( m_scrolledWindow3 );
	tilesetSizer->Add( m_scrolledWindow3, 1, wxEXPAND | wxALL, 0 );
	
	
	sharedSizer->Add( tilesetSizer, 1, wxEXPAND, 5 );
	
	paletteSizer = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, _("Palette") ), wxVERTICAL );
	
	m_scrolledWindow2 = new wxScrolledWindow( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow2->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	paletteBitmap = new wxStaticBitmap( m_scrolledWindow2, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer14->Add( paletteBitmap, 1, wxALL|wxEXPAND, 0 );
	
	
	m_scrolledWindow2->SetSizer( bSizer14 );
	m_scrolledWindow2->Layout();
	bSizer14->Fit( m_scrolledWindow2 );
	paletteSizer->Add( m_scrolledWindow2, 1, wxEXPAND | wxALL, 0 );
	
	
	sharedSizer->Add( paletteSizer, 1, wxEXPAND, 5 );
	
	
	bSizer10->Add( sharedSizer, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, _("Graphics") ), wxVERTICAL );
	
	m_scrolledWindow1 = new wxScrolledWindow( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow1->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	graphicsBitmap = new wxStaticBitmap( m_scrolledWindow1, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( graphicsBitmap, 1, wxALL|wxEXPAND, 0 );
	
	
	m_scrolledWindow1->SetSizer( bSizer13 );
	m_scrolledWindow1->Layout();
	bSizer13->Fit( m_scrolledWindow1 );
	sbSizer2->Add( m_scrolledWindow1, 1, wxEXPAND | wxALL, 0 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer12->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button8 = new wxButton( m_panel3, wxID_ANY, _("<="), wxDefaultPosition, wxDefaultSize, 0 );
	m_button8->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	bSizer12->Add( m_button8, 0, wxALL, 5 );
	
	m_button9 = new wxButton( m_panel3, wxID_ANY, _("=>"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_button9, 0, wxALL, 5 );
	
	
	bSizer12->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	sbSizer2->Add( bSizer12, 0, wxEXPAND, 5 );
	
	
	bSizer10->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	
	m_panel3->SetSizer( bSizer10 );
	m_panel3->Layout();
	bSizer10->Fit( m_panel3 );
	bSizer9->Add( m_panel3, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer9 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitViewerGUI::OnPrev ), NULL, this );
	m_button9->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitViewerGUI::OnNext ), NULL, this );
}

Nin10KitViewerGUI::~Nin10KitViewerGUI()
{
	// Disconnect Events
	m_button8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitViewerGUI::OnPrev ), NULL, this );
	m_button9->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitViewerGUI::OnNext ), NULL, this );
	
}
