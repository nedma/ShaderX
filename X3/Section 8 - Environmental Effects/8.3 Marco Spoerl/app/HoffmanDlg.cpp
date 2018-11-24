/////////////////////////////////////////////////////////////////////////////
// Name:        HoffmanDlg.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:11:53
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "HoffmanDlg.hpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
////@end includes

#include "HoffmanDlg.hpp"

////@begin XPM images
////@end XPM images

/*!
 * Custom event definition
 */

DEFINE_EVENT_TYPE(wxEVT_MY_HOFFMAN_CHANGED)

/*!
 * HoffmanDlg type definition
 */

IMPLEMENT_CLASS( HoffmanDlg, wxDialog )

/*!
 * HoffmanDlg event table definition
 */

BEGIN_EVENT_TABLE( HoffmanDlg, wxDialog )

////@begin HoffmanDlg event table entries
    EVT_SLIDER( ID_SLIDER_SUN_INTENSITY, HoffmanDlg::OnSliderSunIntensityUpdated )

    EVT_SLIDER( ID_SLIDER_MIE, HoffmanDlg::OnSliderMieUpdated )

    EVT_SLIDER( ID_SLIDER_RAYLEIGH, HoffmanDlg::OnSliderRayleighUpdated )

    EVT_SLIDER( ID_SLIDER_HG, HoffmanDlg::OnSliderHgUpdated )

    EVT_SLIDER( ID_SLIDER_INSCATTERING, HoffmanDlg::OnSliderInscatteringUpdated )

    EVT_SLIDER( ID_SLIDER_TURBIDITY, HoffmanDlg::OnSliderTurbidityUpdated )

    EVT_BUTTON( ID_BUTTON_CLOSE, HoffmanDlg::OnButtonCloseClick )

////@end HoffmanDlg event table entries

END_EVENT_TABLE()

/*!
 * HoffmanDlg constructors
 */

HoffmanDlg::HoffmanDlg( )
{
}

HoffmanDlg::HoffmanDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * HoffmanDlg creator
 */

bool HoffmanDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin HoffmanDlg member initialisation
////@end HoffmanDlg member initialisation

////@begin HoffmanDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end HoffmanDlg creation
    return TRUE;
}

/*!
 * Control creation for HoffmanDlg
 */

void HoffmanDlg::CreateControls()
{    
////@begin HoffmanDlg content construction

    HoffmanDlg* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxStaticBox* item3Static = new wxStaticBox(item1, wxID_ANY, _("Atmosphere"));
    wxStaticBoxSizer* item3 = new wxStaticBoxSizer(item3Static, wxVERTICAL);
    item2->Add(item3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxFlexGridSizer* item4 = new wxFlexGridSizer(6, 3, 0, 0);
    item3->Add(item4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item5 = new wxStaticText( item1, wxID_STATIC, _("Sun intensity"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item5, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item6 = new wxSlider( item1, ID_SLIDER_SUN_INTENSITY, 100, 0, 100, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item4->Add(item6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item7 = new wxTextCtrl( item1, ID_TEXTCTRL_SUN_INTENSITY, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item4->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item8 = new wxStaticText( item1, wxID_STATIC, _("Mie scattering coefficient"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item8, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item9 = new wxSlider( item1, ID_SLIDER_MIE, 6, 0, 100, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item4->Add(item9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item10 = new wxTextCtrl( item1, ID_TEXTCTRL_MIE, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item4->Add(item10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item11 = new wxStaticText( item1, wxID_STATIC, _("Rayleigh scattering coefficient"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item12 = new wxSlider( item1, ID_SLIDER_RAYLEIGH, 300, 0, 1000, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item4->Add(item12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item13 = new wxTextCtrl( item1, ID_TEXTCTRL_RAYLEIGH, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item4->Add(item13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item14 = new wxStaticText( item1, wxID_STATIC, _("HG g"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item14, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item15 = new wxSlider( item1, ID_SLIDER_HG, 90, 0, 100, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item4->Add(item15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item16 = new wxTextCtrl( item1, ID_TEXTCTRL_HG, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item4->Add(item16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item17 = new wxStaticText( item1, wxID_STATIC, _("Inscattering"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item17, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item18 = new wxSlider( item1, ID_SLIDER_INSCATTERING, 100, 0, 100, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item4->Add(item18, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item19 = new wxTextCtrl( item1, ID_TEXTCTRL_INSCATTERING, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item4->Add(item19, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item20 = new wxStaticText( item1, wxID_STATIC, _("Turbitiy"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item20, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxSlider* item21 = new wxSlider( item1, ID_SLIDER_TURBIDITY, 1, 1, 6, wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL );
    item4->Add(item21, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* item22 = new wxTextCtrl( item1, ID_TEXTCTRL_TURBIDITY, _T(""), wxDefaultPosition, wxSize(75, -1), wxTE_READONLY );
    item4->Add(item22, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* item23 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item23, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item24 = new wxButton( item1, ID_BUTTON_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add(item24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end HoffmanDlg content construction

    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_SUN_INTENSITY
 */

void HoffmanDlg::OnSliderSunIntensityUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_MIE
 */

void HoffmanDlg::OnSliderMieUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_RAYLEIGH
 */

void HoffmanDlg::OnSliderRayleighUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_HG
 */

void HoffmanDlg::OnSliderHgUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_INSCATTERING
 */

void HoffmanDlg::OnSliderInscatteringUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_TURBIDITY
 */

void HoffmanDlg::OnSliderTurbidityUpdated( wxCommandEvent& event )
{
    UpdateData();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
 */

void HoffmanDlg::OnButtonCloseClick( wxCommandEvent& event )
{
    Close();
}

/*!
 * Should we show tooltips?
 */

bool HoffmanDlg::ShowToolTips()
{
    return TRUE;
}

wxWindow* HoffmanDlg::FindChild(const long id)
{
    for( size_t i = 0; i < GetChildren().GetCount(); ++i )
        if( id == GetChildren().Item( i )->GetData()->GetId() )
            return GetChildren().Item( i )->GetData();

    return NULL;
}

void HoffmanDlg::UpdateData()
{
    // Update sun intensity

    wxSlider* pIntensity = static_cast<wxSlider*>( FindChild( ID_SLIDER_SUN_INTENSITY ) );

    if( NULL != pIntensity )
    {
        m_intensity = pIntensity->GetValue() * 0.01;

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_SUN_INTENSITY ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_intensity;

            pText->SetValue( strText );
        }
    }

    // Update Mie

    wxSlider* pMie = static_cast<wxSlider*>( FindChild( ID_SLIDER_MIE ) );

    if( NULL != pMie )
    {
        m_mie = pMie->GetValue() * 0.0001;

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_MIE ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_mie;

            pText->SetValue( strText );
        }
    }

    // Update Rayleigh

    wxSlider* pRayleigh = static_cast<wxSlider*>( FindChild( ID_SLIDER_RAYLEIGH ) );

    if( NULL != pRayleigh )
    {
        m_rayleigh = pRayleigh->GetValue() * 0.01;

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_RAYLEIGH ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_rayleigh;

            pText->SetValue( strText );
        }
    }

    // Update Hg

    wxSlider* pHg = static_cast<wxSlider*>( FindChild( ID_SLIDER_HG ) );

    if( NULL != pHg )
    {
        m_HGg = pHg->GetValue() * 0.01;

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_HG ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_HGg;

            pText->SetValue( strText );
        }
    }

    // Update Inscattering

    wxSlider* pInscattering = static_cast<wxSlider*>( FindChild( ID_SLIDER_INSCATTERING ) );

    if( NULL != pInscattering )
    {
        m_inscattering = pInscattering->GetValue() * 0.01;

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_INSCATTERING ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_inscattering;

            pText->SetValue( strText );
        }
    }

    // Update Turbidity

    wxSlider* pTurbidity = static_cast<wxSlider*>( FindChild( ID_SLIDER_TURBIDITY ) );

    if( NULL != pTurbidity )
    {
        m_turbidity = pTurbidity->GetValue();

        wxTextCtrl* pText = static_cast<wxTextCtrl*>( FindChild( ID_TEXTCTRL_TURBIDITY ) );

        if( NULL != pText )
        {
            wxString strText;
                     strText << m_turbidity;

            pText->SetValue( strText );
        }
    }

    // Send change event to notify main form

    wxCommandEvent eventCustom( wxEVT_MY_HOFFMAN_CHANGED );
    wxPostEvent( GetParent(), eventCustom );
}
