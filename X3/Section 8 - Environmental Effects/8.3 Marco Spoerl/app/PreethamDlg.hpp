/////////////////////////////////////////////////////////////////////////////
// Name:        PreethamDlg.hpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:11:22
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PREETHAMDLG_H_
#define _PREETHAMDLG_H_

#ifdef __GNUG__
#pragma interface "PreethamDlg.cpp"
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
#define ID_DIALOG_PREETHAM 10000
#define SYMBOL_PREETHAMDLG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_PREETHAMDLG_TITLE _("Preetham")
#define SYMBOL_PREETHAMDLG_IDNAME ID_DIALOG_PREETHAM
#define SYMBOL_PREETHAMDLG_SIZE wxSize(400, 300)
#define SYMBOL_PREETHAMDLG_POSITION wxPoint(0, 0)
#define ID_SLIDER_TURBIDITY 10002
#define ID_TEXTCTRL_TURBIDITY 10003
#define ID_SLIDER_EXPOSURE 10004
#define ID_TEXTCTRL_EXPOSURE 10005
#define ID_SLIDER_INSCATTER 10006
#define ID_TEXTCTRL_INSCATTER 10007
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
    DECLARE_EVENT_TYPE(wxEVT_MY_PREETHAM_CHANGED, 7777)
END_DECLARE_EVENT_TYPES()

#define EVT_MY_PREETHAM_CHANGED(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_MY_PREETHAM_CHANGED, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
        (wxObject *) NULL \
        ),

/*!
 * PreethamDlg class declaration
 */

class PreethamDlg: public wxDialog
{    

    DECLARE_CLASS( PreethamDlg )
    DECLARE_EVENT_TABLE()

private:

////@begin PreethamDlg member variables
    float m_turbidity;
    float m_exposure;
    float m_scaleInscatter;
////@end PreethamDlg member variables

    wxWindow*   FindChild   ( const long id );
    void        UpdateData  ();

public:

    /// Constructors
    PreethamDlg( );
    PreethamDlg( wxWindow* parent, wxWindowID id = SYMBOL_PREETHAMDLG_IDNAME, const wxString& caption = SYMBOL_PREETHAMDLG_TITLE, const wxPoint& pos = SYMBOL_PREETHAMDLG_POSITION, const wxSize& size = SYMBOL_PREETHAMDLG_SIZE, long style = SYMBOL_PREETHAMDLG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PREETHAMDLG_IDNAME, const wxString& caption = SYMBOL_PREETHAMDLG_TITLE, const wxPoint& pos = SYMBOL_PREETHAMDLG_POSITION, const wxSize& size = SYMBOL_PREETHAMDLG_SIZE, long style = SYMBOL_PREETHAMDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin PreethamDlg event handler declarations

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_TURBIDITY
    void OnSliderTurbidityUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_EXPOSURE
    void OnSliderExposureUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_INSCATTER
    void OnSliderInscatterUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
    void OnButtonCloseClick( wxCommandEvent& event );

////@end PreethamDlg event handler declarations

////@begin PreethamDlg member function declarations

    float GetTurbidity() const { return m_turbidity ; }
    void SetTurbidity(float value) { m_turbidity = value ; }

    float GetExposure() const { return m_exposure ; }
    void SetExposure(float value) { m_exposure = value ; }

    float GetScaleInscatter() const { return m_scaleInscatter ; }
    void SetScaleInscatter(float value) { m_scaleInscatter = value ; }

////@end PreethamDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

};

#endif
    // _PREETHAMDLG_H_
