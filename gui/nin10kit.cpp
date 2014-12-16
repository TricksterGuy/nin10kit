///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "nin10kit.h"

///////////////////////////////////////////////////////////////////////////

Nin10KitFrame::Nin10KitFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( Nin10KitFrame::m_splitter1OnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	imagesWindow = new wxScrolledWindow( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	imagesWindow->SetScrollRate( 5, 5 );
	wxFlexGridSizer* imagesSizer;
	imagesSizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	imagesSizer->SetFlexibleDirection( wxBOTH );
	imagesSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	imagesWindow->SetSizer( imagesSizer );
	imagesWindow->Layout();
	imagesSizer->Fit( imagesWindow );
	bSizer2->Add( imagesWindow, 1, wxEXPAND | wxALL, 0 );
	
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer3->Add( bSizer5, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 0 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText7 = new wxStaticText( m_panel2, wxID_ANY, _("Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	bSizer6->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 8 );
	
	wxString modeChoices[] = { _("GBA Mode 3 - 16 Bpp Bitmap BGR"), _("GBA Mode 4 - 8 Bpp Bitmap w/ Palette"), _("GBA Mode 0 - 8 Bpp Map+Tileset w/ Palette"), _("GBA Mode 0 - 4 Bpp Map+Tileset w/ 16 Palette Banks"), _("GBA Sprites - 8 Bpp Sprites w/ Palette"), _("GBA Sprites - 4 Bpp Sprites w/ 16 Palette Banks") };
	int modeNChoices = sizeof( modeChoices ) / sizeof( wxString );
	mode = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, modeNChoices, modeChoices, 0 );
	mode->SetSelection( 0 );
	bSizer6->Add( mode, 1, wxALL, 8 );
	
	
	bSizer3->Add( bSizer6, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button2 = new wxButton( m_panel2, wxID_ANY, _("Load Images"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button2, 0, wxALL, 5 );
	
	m_button3 = new wxButton( m_panel2, wxID_ANY, _("Delete All Images"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button3, 0, wxALL, 5 );
	
	
	bSizer3->Add( bSizer8, 0, wxEXPAND, 5 );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, _("Image Info") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panel2, wxID_ANY, _("Filename:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer2->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 8 );
	
	imageFilename = new wxStaticText( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	imageFilename->Wrap( -1 );
	fgSizer2->Add( imageFilename, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 8 );
	
	m_staticText3 = new wxStaticText( m_panel2, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 8 );
	
	imageName = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( imageName, 0, wxEXPAND|wxALL, 8 );
	
	m_staticText5 = new wxStaticText( m_panel2, wxID_ANY, _("Size (W x H):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer2->Add( m_staticText5, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 8 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	imageWidth = new wxSpinCtrl( m_panel2, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( -1,-1 ), wxSP_ARROW_KEYS, 0, 8192, 0 );
	imageWidth->SetMaxSize( wxSize( 80,-1 ) );
	
	bSizer4->Add( imageWidth, 0, wxALL, 8 );
	
	m_staticText6 = new wxStaticText( m_panel2, wxID_ANY, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer4->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 8 );
	
	imageHeight = new wxSpinCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 8192, 0 );
	imageHeight->SetMaxSize( wxSize( 80,-1 ) );
	
	bSizer4->Add( imageHeight, 0, wxALL, 8 );
	
	
	fgSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( m_panel2, wxID_ANY, _("Animated:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 8 );
	
	imageAnimated = new wxCheckBox( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	imageAnimated->Enable( false );
	
	fgSizer2->Add( imageAnimated, 0, wxALL, 5 );
	
	
	fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button1 = new wxButton( m_panel2, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_button1, 0, wxALL|wxALIGN_RIGHT, 8 );
	
	
	sbSizer1->Add( fgSizer2, 1, wxEXPAND, 0 );
	
	
	bSizer3->Add( sbSizer1, 0, wxEXPAND, 8 );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer9->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button4 = new wxButton( m_panel2, wxID_ANY, _("Export"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_button4, 0, wxALL, 5 );
	
	m_button5 = new wxButton( m_panel2, wxID_ANY, _("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_button5, 0, wxALL, 5 );
	
	
	bSizer3->Add( bSizer9, 0, wxEXPAND, 5 );
	
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	m_splitter1->SplitVertically( m_panel1, m_panel2, 320 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnLoadImages ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnDeleteAllImages ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnDeleteCurrentImage ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnExport ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnEditor ), NULL, this );
}

Nin10KitFrame::~Nin10KitFrame()
{
	// Disconnect Events
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnLoadImages ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnDeleteAllImages ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnDeleteCurrentImage ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnExport ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Nin10KitFrame::OnEditor ), NULL, this );
	
}
