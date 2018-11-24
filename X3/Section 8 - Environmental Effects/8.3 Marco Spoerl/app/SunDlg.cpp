/////////////////////////////////////////////////////////////////////////////
// Name:        SunDlg.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:16:51
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "SunDlg.hpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
////@end includes

#include "SunDlg.hpp"

////@begin XPM images
////@end XPM images

/*!
 * Custom event definition
 */

DEFINE_EVENT_TYPE(wxEVT_MY_SUN_CHANGED)

/*!
 * SunDlg type definition
 */

IMPLEMENT_CLASS( SunDlg, wxDialog )

/*!
 * SunDlg event table definition
 */

BEGIN_EVENT_TABLE( SunDlg, wxDialog )

////@begin SunDlg event table entries
    EVT_SLIDER( ID_SLIDER_POSITION, SunDlg::OnSliderPositionUpdated )

    EVT_BUTTON( ID_BUTTON_CLOSE, SunDlg::OnButtonCloseClick )

////@end SunDlg event table entries

END_EVENT_TABLE()

/*!
 * SunDlg constructors
 */

SunDlg::SunDlg( )
{
}

SunDlg::SunDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * SunDlg creator
 */

bool SunDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin SunDlg member initialisation
////@end SunDlg member initialisation

////@begin SunDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end SunDlg creation
    return TRUE;
}

/*!
 * Control creation for SunDlg
 */

void SunDlg::CreateControls()
{    
////@begin SunDlg content construction

    SunDlg* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxStaticBox* item3Static = new wxStaticBox(item1, wxID_ANY, _("Position"));
    wxStaticBoxSizer* item3 = new wxStaticBoxSizer(item3Static, wxHORIZONTAL);
    item2->Add(item3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item4 = new wxStaticText( item1, wxID_STATIC, _("East"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item4, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item5 = new wxSlider( item1, ID_SLIDER_POSITION, 70, 0, 180, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    item3->Add(item5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item6 = new wxStaticText( item1, wxID_STATIC, _("West"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item6, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* item7 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item8 = new wxButton( item1, ID_BUTTON_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add(item8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end SunDlg content construction

    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_POSITION
 */

void SunDlg::OnSliderPositionUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
 */

void SunDlg::OnButtonCloseClick( wxCommandEvent& event )
{
    Close();
}

/*!
 * Should we show tooltips?
 */

bool SunDlg::ShowToolTips()
{
    return TRUE;
}

wxWindow* SunDlg::FindChild(const long id)
{
    for( size_t i = 0; i < GetChildren().GetCount(); ++i )
        if( id == GetChildren().Item( i )->GetData()->GetId() )
            return GetChildren().Item( i )->GetData();

    return NULL;
}

void SunDlg::UpdateData()
{
    // Update position

    wxSlider* pPosition = static_cast<wxSlider*>( FindChild( ID_SLIDER_POSITION ) );

    if( NULL != pPosition )
        m_position = pPosition->GetValue();

    // Send change event to notify main form

    wxCommandEvent eventCustom( wxEVT_MY_SUN_CHANGED );
    wxPostEvent( GetParent(), eventCustom );
}
