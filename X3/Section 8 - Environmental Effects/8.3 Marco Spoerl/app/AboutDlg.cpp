/////////////////////////////////////////////////////////////////////////////
// Name:        AboutDlg.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:10:51
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "AboutDlg.hpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
////@end includes

#include "AboutDlg.hpp"

////@begin XPM images
////@end XPM images

/*!
 * AboutDlg type definition
 */

IMPLEMENT_CLASS( AboutDlg, wxDialog )

/*!
 * AboutDlg event table definition
 */

BEGIN_EVENT_TABLE( AboutDlg, wxDialog )

////@begin AboutDlg event table entries
    EVT_BUTTON( ID_BUTTON_CLOSE, AboutDlg::OnButtonCloseClick )

////@end AboutDlg event table entries

END_EVENT_TABLE()

/*!
 * AboutDlg constructors
 */

AboutDlg::AboutDlg( )
{
}

AboutDlg::AboutDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * AboutDlg creator
 */

bool AboutDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin AboutDlg member initialisation
////@end AboutDlg member initialisation

////@begin AboutDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end AboutDlg creation
    return TRUE;
}

/*!
 * Control creation for AboutDlg
 */

void AboutDlg::CreateControls()
{    
////@begin AboutDlg content construction

    AboutDlg* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxStaticBox* item3Static = new wxStaticBox(item1, wxID_ANY, _T(""));
    wxStaticBoxSizer* item3 = new wxStaticBoxSizer(item3Static, wxVERTICAL);
    item2->Add(item3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item4 = new wxStaticText( item1, wxID_STATIC, _("Daysky rendering"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* item5 = new wxStaticText( item1, wxID_STATIC, _("Real-time pyhsically-based skylight and aerial perspective"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* item6 = new wxStaticText( item1, wxID_STATIC, _("(c) 2004 Marco Spoerl"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* item7 = new wxStaticText( item1, wxID_STATIC, _("http://www.marcospoerl.com"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* item8 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item9 = new wxButton( item1, ID_BUTTON_CLOSE, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add(item9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end AboutDlg content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
 */

void AboutDlg::OnButtonCloseClick( wxCommandEvent& event )
{
    Close();
}

/*!
 * Should we show tooltips?
 */

bool AboutDlg::ShowToolTips()
{
    return TRUE;
}
