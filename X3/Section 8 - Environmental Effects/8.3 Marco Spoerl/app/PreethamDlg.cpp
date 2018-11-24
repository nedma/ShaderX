/////////////////////////////////////////////////////////////////////////////
// Name:        PreethamDlg.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:11:22
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "PreethamDlg.hpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
////@end includes

#include "PreethamDlg.hpp"

////@begin XPM images
////@end XPM images

/*!
 * Custom event definition
 */

DEFINE_EVENT_TYPE(wxEVT_MY_PREETHAM_CHANGED)

/*!
 * PreethamDlg type definition
 */

IMPLEMENT_CLASS( PreethamDlg, wxDialog )

/*!
 * PreethamDlg event table definition
 */

BEGIN_EVENT_TABLE( PreethamDlg, wxDialog )

////@begin PreethamDlg event table entries
    EVT_SLIDER( ID_SLIDER_TURBIDITY, PreethamDlg::OnSliderTurbidityUpdated )

    EVT_SLIDER( ID_SLIDER_EXPOSURE, PreethamDlg::OnSliderExposureUpdated )

    EVT_SLIDER( ID_SLIDER_INSCATTER, PreethamDlg::OnSliderInscatterUpdated )

    EVT_BUTTON( ID_BUTTON_CLOSE, PreethamDlg::OnButtonCloseClick )

////@end PreethamDlg event table entries

END_EVENT_TABLE()

/*!
 * PreethamDlg constructors
 */

PreethamDlg::PreethamDlg( )
{
}

PreethamDlg::PreethamDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * PreethamDlg creator
 */

bool PreethamDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PreethamDlg member initialisation
////@end PreethamDlg member initialisation

////@begin PreethamDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end PreethamDlg creation
    return TRUE;
}

/*!
 * Control creation for PreethamDlg
 */

void PreethamDlg::CreateControls()
{    
////@begin PreethamDlg content construction

    PreethamDlg* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxStaticBox* item3Static = new wxStaticBox(item1, wxID_ANY, _("Atmosphere"));
    wxStaticBoxSizer* item3 = new wxStaticBoxSizer(item3Static, wxVERTICAL);
    item2->Add(item3, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* item4 = new wxFlexGridSizer(3, 3, 0, 0);
    item3->Add(item4, 0, wxGROW|wxALL, 5);

    wxStaticText* item5 = new wxStaticText( item1, wxID_STATIC, _("Turbidity"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item5, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item6 = new wxSlider( item1, ID_SLIDER_TURBIDITY, 2, 2, 6, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item4->Add(item6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item7 = new wxTextCtrl( item1, ID_TEXTCTRL_TURBIDITY, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item4->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* item8Static = new wxStaticBox(item1, wxID_ANY, _("Scaling"));
    wxStaticBoxSizer* item8 = new wxStaticBoxSizer(item8Static, wxVERTICAL);
    item2->Add(item8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxFlexGridSizer* item9 = new wxFlexGridSizer(2, 3, 0, 0);
    item8->Add(item9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item10 = new wxStaticText( item1, wxID_STATIC, _("Exposure"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add(item10, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item11 = new wxSlider( item1, ID_SLIDER_EXPOSURE, 10, 1, 100, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item9->Add(item11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item12 = new wxTextCtrl( item1, ID_TEXTCTRL_EXPOSURE, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item9->Add(item12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item13 = new wxStaticText( item1, wxID_STATIC, _("Inscattering scale"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add(item13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item14 = new wxSlider( item1, ID_SLIDER_INSCATTER, 70, 1, 1000, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item9->Add(item14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item15 = new wxTextCtrl( item1, ID_TEXTCTRL_INSCATTER, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item9->Add(item15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* item16 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item16, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item17 = new wxButton( item1, ID_BUTTON_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end PreethamDlg content construction

    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_TURBIDITY
 */

void PreethamDlg::OnSliderTurbidityUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_SCALE_CIEY
 */

void PreethamDlg::OnSliderExposureUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_SCALE_INSCATTER
 */

void PreethamDlg::OnSliderInscatterUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
 */

void PreethamDlg::OnButtonCloseClick( wxCommandEvent& event )
{
    Close();
}

/*!
 * Should we show tooltips?
 */

bool PreethamDlg::ShowToolTips()
{
    return TRUE;
}

wxWindow* PreethamDlg::FindChild(const long id)
{
    for( size_t i = 0; i < GetChildren().GetCount(); ++i )
        if( id == GetChildren().Item( i )->GetData()->GetId() )
            return GetChildren().Item( i )->GetData();

    return NULL;
}

void PreethamDlg::UpdateData()
{
    // Update turbidity

    wxSlider* pTurbidity = static_cast<wxSlider*>( FindChild( ID_SLIDER_TURBIDITY ) );

    if( NULL != pTurbidity )
    {
        m_turbidity = pTurbidity->GetValue() * 1.0f;

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_TURBIDITY ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_turbidity;

            pText->SetValue( strText );
        }
    }

    // Update exposure

    wxSlider* pExposure = static_cast<wxSlider*>( FindChild( ID_SLIDER_EXPOSURE ) );

    if( NULL != pExposure )
    {
        m_exposure = 1.0 / pExposure->GetValue();

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_EXPOSURE ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_exposure;

            pText->SetValue( strText );
        }
    }

    // Update inscatter scale

    wxSlider* pInscatter = static_cast<wxSlider*>( FindChild( ID_SLIDER_INSCATTER ) );

    if( NULL != pInscatter )
    {
        m_scaleInscatter = pInscatter->GetValue() * 0.0001f;

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_INSCATTER ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_scaleInscatter;

            pText->SetValue( strText );
        }
    }

    // Send change event to notify main form

    wxCommandEvent eventCustom( wxEVT_MY_PREETHAM_CHANGED );
    wxPostEvent( GetParent(), eventCustom );
}
