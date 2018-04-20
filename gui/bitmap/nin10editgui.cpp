///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 23 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "color_info_panel.hpp"
#include "palette_editor_panel.hpp"

#include "nin10editgui.h"

///////////////////////////////////////////////////////////////////////////

Nin10EditGUI::Nin10EditGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 640,480 ), wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel2 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	graphics_notebook = new wxNotebook( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_FLAT|wxNB_MULTILINE|wxNB_NOPAGETHEME );
	
	bSizer5->Add( graphics_notebook, 1, wxEXPAND, 5 );
	
	
	m_panel2->SetSizer( bSizer5 );
	m_panel2->Layout();
	bSizer5->Fit( m_panel2 );
	bSizer3->Add( m_panel2, 1, wxEXPAND, 5 );
	
	m_panel3 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	palette_sizer = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, wxT("Palette") ), wxVERTICAL );
	
	palette_panel = new PaletteEditorPanel( palette_sizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	palette_panel->SetScrollRate( 1, 1 );
	palette_panel->SetMinSize( wxSize( 256,256 ) );
	palette_panel->SetMaxSize( wxSize( 256,256 ) );
	
	palette_sizer->Add( palette_panel, 0, wxALL, 0 );
	
	
	bSizer4->Add( palette_sizer, 0, 0, 5 );
	
	color_info = new ColorInfoPanel( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	color_info->SetScrollRate( 1, 1 );
	color_info->SetMinSize( wxSize( 256,128 ) );
	color_info->SetMaxSize( wxSize( 256,128 ) );
	
	bSizer4->Add( color_info, 0, wxALL, 5 );
	
	
	m_panel3->SetSizer( bSizer4 );
	m_panel3->Layout();
	bSizer4->Fit( m_panel3 );
	bSizer3->Add( m_panel3, 1, wxEXPAND, 5 );
	
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	menu_bar = new wxMenuBar( 0 );
	file = new wxMenu();
	wxMenuItem* fileNew;
	fileNew = new wxMenuItem( file, ID_NEW, wxString( wxT("New") ) + wxT('\t') + wxT("Ctrl+N"), wxEmptyString, wxITEM_NORMAL );
	file->Append( fileNew );
	
	wxMenuItem* open;
	open = new wxMenuItem( file, ID_OPEN, wxString( wxT("Open") ) + wxT('\t') + wxT("Ctrl+O"), wxEmptyString, wxITEM_NORMAL );
	file->Append( open );
	
	wxMenuItem* save;
	save = new wxMenuItem( file, ID_SAVE, wxString( wxT("Save") ) + wxT('\t') + wxT("Ctrl+S"), wxEmptyString, wxITEM_NORMAL );
	file->Append( save );
	
	wxMenuItem* saveAs;
	saveAs = new wxMenuItem( file, ID_SAVE_AS, wxString( wxT("Save As") ) + wxT('\t') + wxT("F12"), wxEmptyString, wxITEM_NORMAL );
	file->Append( saveAs );
	
	file->AppendSeparator();
	
	wxMenuItem* quit;
	quit = new wxMenuItem( file, ID_QUIT, wxString( wxT("Quit") ) + wxT('\t') + wxT("Ctrl+Q"), wxEmptyString, wxITEM_NORMAL );
	file->Append( quit );
	
	menu_bar->Append( file, wxT("File") ); 
	
	edit = new wxMenu();
	menu_bar->Append( edit, wxT("Edit") ); 
	
	view = new wxMenu();
	menu_bar->Append( view, wxT("View") ); 
	
	tools = new wxMenu();
	menu_bar->Append( tools, wxT("Tools") ); 
	
	help = new wxMenu();
	menu_bar->Append( help, wxT("Help") ); 
	
	this->SetMenuBar( menu_bar );
	
	wxToolBar* toolbar;
	toolbar = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY ); 
	toolbar->Realize(); 
	
	status_bar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	this->Connect( fileNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnNew ) );
	this->Connect( open->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnOpen ) );
	this->Connect( save->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnSave ) );
	this->Connect( saveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnSaveAs ) );
	this->Connect( quit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnQuit ) );
}

Nin10EditGUI::~Nin10EditGUI()
{
	// Disconnect Events
	this->Disconnect( ID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnNew ) );
	this->Disconnect( ID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnOpen ) );
	this->Disconnect( ID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnSave ) );
	this->Disconnect( ID_SAVE_AS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnSaveAs ) );
	this->Disconnect( ID_QUIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Nin10EditGUI::OnQuit ) );
	
}
