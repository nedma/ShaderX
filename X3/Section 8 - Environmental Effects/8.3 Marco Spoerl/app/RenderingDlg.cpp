/////////////////////////////////////////////////////////////////////////////
// Name:        RenderingDlg.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:16:45
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "RenderingDlg.hpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
////@end includes

#include "RenderingDlg.hpp"

////@begin XPM images

////@end XPM images

/*!
 * Custom event definition
 */

DEFINE_EVENT_TYPE(wxEVT_MY_RENDERING_CHANGED)

/*!
 * RenderingDlg type definition
 */

IMPLEMENT_CLASS( RenderingDlg, wxDialog )

/*!
 * RenderingDlg event table definition
 */

BEGIN_EVENT_TABLE( RenderingDlg, wxDialog )

////@begin RenderingDlg event table entries
    EVT_CHECKBOX( ID_CHECKBOX_WIREFRAME, RenderingDlg::OnCheckboxWireframeClick )

    EVT_SLIDER( ID_SLIDER_RINGS, RenderingDlg::OnSliderRingsUpdated )

    EVT_SLIDER( ID_SLIDER_SEGMENTS, RenderingDlg::OnSliderSegmentsUpdated )

    EVT_BUTTON( ID_BUTTON_CLOSE, RenderingDlg::OnButtonCloseClick )

////@end RenderingDlg event table entries

END_EVENT_TABLE()

/*!
 * RenderingDlg constructors
 */

RenderingDlg::RenderingDlg( )
{
}

RenderingDlg::RenderingDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * RenderingDlg creator
 */

bool RenderingDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin RenderingDlg member initialisation
////@end RenderingDlg member initialisation

////@begin RenderingDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end RenderingDlg creation
    return TRUE;
}

/*!
 * Control creation for RenderingDlg
 */

void RenderingDlg::CreateControls()
{    
////@begin RenderingDlg content construction

    RenderingDlg* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxStaticBox* item3Static = new wxStaticBox(item1, wxID_ANY, _("General"));
    wxStaticBoxSizer* item3 = new wxStaticBoxSizer(item3Static, wxVERTICAL);
    item2->Add(item3, 0, wxGROW|wxALL, 5);

    wxCheckBox* item4 = new wxCheckBox( item1, ID_CHECKBOX_WIREFRAME, _("Render as wireframe"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->SetValue(FALSE);
    item3->Add(item4, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticBox* item5Static = new wxStaticBox(item1, wxID_ANY, _("Sky dome"));
    wxStaticBoxSizer* item5 = new wxStaticBoxSizer(item5Static, wxVERTICAL);
    item2->Add(item5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxFlexGridSizer* item6 = new wxFlexGridSizer(2, 3, 0, 0);
    item5->Add(item6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item7 = new wxStaticText( item1, wxID_STATIC, _("Rings"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add(item7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item8 = new wxSlider( item1, ID_SLIDER_RINGS, 128, 1, 256, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item6->Add(item8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item9 = new wxTextCtrl( item1, ID_TEXTCTRL_RINGS, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item6->Add(item9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item10 = new wxStaticText( item1, wxID_STATIC, _("Segments"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add(item10, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item11 = new wxSlider( item1, ID_SLIDER_SEGMENTS, 128, 4, 256, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item6->Add(item11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item12 = new wxTextCtrl( item1, ID_TEXTCTRL_SEGMENTS, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item6->Add(item12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* item13 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item13, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item14 = new wxButton( item1, ID_BUTTON_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add(item14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end RenderingDlg content construction

    UpdateData();
}

/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_WIREFRAME
 */

void RenderingDlg::OnCheckboxWireframeClick( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_RINGS
 */

void RenderingDlg::OnSliderRingsUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_SEGMENTS
 */

void RenderingDlg::OnSliderSegmentsUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
 */

void RenderingDlg::OnButtonCloseClick( wxCommandEvent& event )
{
    Close();
}

/*!
 * Should we show tooltips?
 */

bool RenderingDlg::ShowToolTips()
{
    return TRUE;
}

wxWindow* RenderingDlg::FindChild(const long id)
{
    for( size_t i = 0; i < GetChildren().GetCount(); ++i )
        if( id == GetChildren().Item( i )->GetData()->GetId() )
            return GetChildren().Item( i )->GetData();

    return NULL;
}

void RenderingDlg::UpdateData()
{
    // Update ring count

    wxSlider* pRings = static_cast<wxSlider*>( FindChild( ID_SLIDER_RINGS ) );

    if( NULL != pRings )
    {
        m_rings = pRings->GetValue();

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_RINGS ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_rings;

            pText->SetValue( strText );
        }
    }

    // Update segment count

    wxSlider* pSegments = static_cast<wxSlider*>( FindChild( ID_SLIDER_SEGMENTS ) );

    if( NULL != pSegments )
    {
        m_segments = pSegments->GetValue();

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_SEGMENTS ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_segments;

            pText->SetValue( strText );
        }
    }

    // Send change event to notify main form

    wxCommandEvent eventCustom( wxEVT_MY_RENDERING_CHANGED );
    wxPostEvent( GetParent(), eventCustom );
}
