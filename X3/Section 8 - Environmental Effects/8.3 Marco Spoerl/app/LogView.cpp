/////////////////////////////////////////////////////////////////////////////
// Name:        LogView.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:16:50
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "LogView.hpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
////@end includes

#include "LogView.hpp"

////@begin XPM images

////@end XPM images

/*!
 * LogView type definition
 */

IMPLEMENT_CLASS( LogView, wxDialog )

/*!
 * LogView event table definition
 */

BEGIN_EVENT_TABLE( LogView, wxDialog )

////@begin LogView event table entries
    EVT_BUTTON( ID_BUTTON_CLOSE, LogView::OnButtonCloseClick )

////@end LogView event table entries

END_EVENT_TABLE()

/*!
 * LogView constructors
 */

LogView::LogView( )
{
}

LogView::LogView( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * LogView creator
 */

bool LogView::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin LogView member initialisation
////@end LogView member initialisation

////@begin LogView creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end LogView creation
    return TRUE;
}

/*!
 * Control creation for LogView
 */

void LogView::CreateControls()
{    
////@begin LogView content construction

    LogView* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxTextCtrl* item3 = new wxTextCtrl( item1, ID_TEXTCTRL_LOG, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
    item2->Add(item3, 1, wxGROW|wxALL, 5);

    wxBoxSizer* item4 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item5 = new wxButton( item1, ID_BUTTON_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end LogView content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
 */

void LogView::OnButtonCloseClick( wxCommandEvent& event )
{
    Close();
}

/*!
 * Should we show tooltips?
 */

bool LogView::ShowToolTips()
{
    return TRUE;
}

wxTextCtrl* LogView::GetTextCtrl    () const
{
    for( size_t i = 0; i < GetChildren().GetCount(); ++i )
        if( ID_TEXTCTRL_LOG == GetChildren().Item( i )->GetData()->GetId() )
            return static_cast<wxTextCtrl*>( GetChildren().Item( i )->GetData() );

    return NULL;
}
