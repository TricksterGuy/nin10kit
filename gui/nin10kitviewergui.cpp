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
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel3 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	sharedSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	tilesetSizer = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, _("Tileset") ), wxVERTICAL );
	
	tilesetWindow = new wxScrolledWindow( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	tilesetWindow->SetScrollRate( 5, 5 );
	tilesetWindow->SetMinSize( wxSize( 256,256 ) );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	tilesetBitmap = new wxStaticBitmap( tilesetWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( tilesetBitmap, 0, wxALL, 0 );
	
	
	tilesetWindow->SetSizer( bSizer15 );
	tilesetWindow->Layout();
	bSizer15->Fit( tilesetWindow );
	tilesetSizer->Add( tilesetWindow, 1, wxEXPAND | wxALL, 0 );
	
	
	bSizer8->Add( tilesetSizer, 0, wxEXPAND, 0 );
	
	
	sharedSizer->Add( bSizer8, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	paletteSizer = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, _("Palette") ), wxVERTICAL );
	
	m_scrolledWindow2 = new wxScrolledWindow( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow2->SetScrollRate( 5, 5 );
	m_scrolledWindow2->SetMinSize( wxSize( 256,256 ) );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	paletteBitmap = new wxStaticBitmap( m_scrolledWindow2, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	paletteBitmap->SetMinSize( wxSize( 256,256 ) );
	
	bSizer14->Add( paletteBitmap, 0, wxALL, 0 );
	
	
	m_scrolledWindow2->SetSizer( bSizer14 );
	m_scrolledWindow2->Layout();
	bSizer14->Fit( m_scrolledWindow2 );
	paletteSizer->Add( m_scrolledWindow2, 1, wxEXPAND | wxALL, 0 );
	
	
	bSizer9->Add( paletteSizer, 0, wxEXPAND, 0 );
	
	
	sharedSizer->Add( bSizer9, 1, wxEXPAND, 5 );
	
	
	bSizer2->Add( sharedSizer, 0, wxEXPAND, 5 );
	
	graphicsSizer = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, _("Graphics") ), wxVERTICAL );
	
	graphicsWindow = new wxScrolledWindow( m_panel3, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxHSCROLL|wxVSCROLL );
	graphicsWindow->SetScrollRate( 5, 5 );
	graphicsWindow->SetMinSize( wxSize( 256,256 ) );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	graphicsBitmap = new wxStaticBitmap( graphicsWindow, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( graphicsBitmap, 0, wxALL, 0 );
	
	
	graphicsWindow->SetSizer( bSizer13 );
	graphicsWindow->Layout();
	bSizer13->Fit( graphicsWindow );
	graphicsSizer->Add( graphicsWindow, 0, wxALL, 0 );
	
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	buttonSizer->SetMinSize( wxSize( 256,-1 ) ); 
	
	buttonSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button8 = new wxButton( m_panel3, wxID_ANY, _("<="), wxDefaultPosition, wxDefaultSize, 0 );
	m_button8->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	buttonSizer->Add( m_button8, 0, wxALL, 5 );
	
	m_button9 = new wxButton( m_panel3, wxID_ANY, _("=>"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_button9, 0, wxALL, 5 );
	
	
	buttonSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	graphicsSizer->Add( buttonSizer, 0, 0, 0 );
	
	
	bSizer2->Add( graphicsSizer, 1, wxEXPAND, 5 );
	
	
	m_panel3->SetSizer( bSizer2 );
	m_panel3->Layout();
	bSizer2->Fit( m_panel3 );
	bSizer1->Add( m_panel3, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer1 );
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
