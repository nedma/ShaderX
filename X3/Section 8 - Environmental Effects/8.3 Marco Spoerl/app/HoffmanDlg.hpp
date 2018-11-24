/////////////////////////////////////////////////////////////////////////////
// Name:        HoffmanDlg.hpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:11:53
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _HOFFMANDLG_H_
#define _HOFFMANDLG_H_

#ifdef __GNUG__
#pragma interface "HoffmanDlg.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/wx.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_HOFFMAN 10000
#define SYMBOL_HOFFMANDLG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_HOFFMANDLG_TITLE _("Hoffman")
#define SYMBOL_HOFFMANDLG_IDNAME ID_DIALOG_HOFFMAN
#define SYMBOL_HOFFMANDLG_SIZE wxSize(400, 300)
#define SYMBOL_HOFFMANDLG_POSITION wxPoint(0, 0)
#define ID_SLIDER_SUN_INTENSITY 10002
#define ID_TEXTCTRL_SUN_INTENSITY 10003
#define ID_SLIDER_MIE 10004
#define ID_TEXTCTRL_MIE 10008
#define ID_SLIDER_RAYLEIGH 10005
#define ID_TEXTCTRL_RAYLEIGH 10009
#define ID_SLIDER_HG 10006
#define ID_TEXTCTRL_HG 10010
#define ID_SLIDER_INSCATTERING 10007
#define ID_TEXTCTRL_INSCATTERING 10013
#define ID_SLIDER_TURBIDITY 10011
#define ID_TEXTCTRL_TURBIDITY 10012
#define ID_BUTTON_CLOSE 10001
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * Custom event declarations
 */

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_MY_HOFFMAN_CHANGED, 7777)
END_DECLARE_EVENT_TYPES()

#define EVT_MY_HOFFMAN_CHANGED(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_MY_HOFFMAN_CHANGED, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
        (wxObject *) NULL \
        ),

/*!
 * HoffmanDlg class declaration
 */

class HoffmanDlg: public wxDialog
{   
    
    DECLARE_CLASS( HoffmanDlg )
    DECLARE_EVENT_TABLE()

private:

////@begin HoffmanDlg member variables
    float m_intensity;
    float m_mie;
    float m_rayleigh;
    float m_HGg;
    float m_inscattering;
    float m_turbidity;
////@end HoffmanDlg member variables

    wxWindow*   FindChild   ( const long id );
    void        UpdateData  ();

public:

    /// Constructors
    HoffmanDlg( );
    HoffmanDlg( wxWindow* parent, wxWindowID id = SYMBOL_HOFFMANDLG_IDNAME, const wxString& caption = SYMBOL_HOFFMANDLG_TITLE, const wxPoint& pos = SYMBOL_HOFFMANDLG_POSITION, const wxSize& size = SYMBOL_HOFFMANDLG_SIZE, long style = SYMBOL_HOFFMANDLG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_HOFFMANDLG_IDNAME, const wxString& caption = SYMBOL_HOFFMANDLG_TITLE, const wxPoint& pos = SYMBOL_HOFFMANDLG_POSITION, const wxSize& size = SYMBOL_HOFFMANDLG_SIZE, long style = SYMBOL_HOFFMANDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin HoffmanDlg event handler declarations

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_SUN_INTENSITY
    void OnSliderSunIntensityUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_MIE
    void OnSliderMieUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_RAYLEIGH
    void OnSliderRayleighUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_HG
    void OnSliderHgUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_INSCATTERING
    void OnSliderInscatteringUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_TURBIDITY
    void OnSliderTurbidityUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
    void OnButtonCloseClick( wxCommandEvent& event );

////@end HoffmanDlg event handler declarations

////@begin HoffmanDlg member function declarations

    float GetIntensity() const { return m_intensity ; }
    void SetIntensity(float value) { m_intensity = value ; }

    float GetMie() const { return m_mie ; }
    void SetMie(float value) { m_mie = value ; }

    float GetRayleigh() const { return m_rayleigh ; }
    void SetRayleigh(float value) { m_rayleigh = value ; }

    float GetHGg() const { return m_HGg ; }
    void SetHGg(float value) { m_HGg = value ; }

    float GetInscattering() const { return m_inscattering ; }
    void SetInscattering(float value) { m_inscattering = value ; }

    float GetTurbidity() const { return m_turbidity ; }
    void SetTurbidity(float value) { m_turbidity = value ; }

////@end HoffmanDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

};

#endif
    // _HOFFMANDLG_H_
