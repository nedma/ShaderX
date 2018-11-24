/////////////////////////////////////////////////////////////////////////////
// Name:        SunDlg.hpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:16:51
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _SUNDLG_H_
#define _SUNDLG_H_

#ifdef __GNUG__
#pragma interface "SunDlg.cpp"
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
#define ID_DIALOG_SUN 10000
#define SYMBOL_SUNDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_SUNDLG_TITLE _("Sun")
#define SYMBOL_SUNDLG_IDNAME ID_DIALOG_SUN
#define SYMBOL_SUNDLG_SIZE wxSize(400, 300)
#define SYMBOL_SUNDLG_POSITION wxPoint(0, 0)
#define ID_SLIDER_POSITION 10002
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
    DECLARE_EVENT_TYPE(wxEVT_MY_SUN_CHANGED, 7777)
END_DECLARE_EVENT_TYPES()

#define EVT_MY_SUN_CHANGED(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_MY_SUN_CHANGED, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
        (wxObject *) NULL \
        ),

/*!
 * SunDlg class declaration
 */

class SunDlg: public wxDialog
{    

    DECLARE_CLASS( SunDlg )
    DECLARE_EVENT_TABLE()

private:

////@begin SunDlg member variables
    int m_position;
////@end SunDlg member variables

    wxWindow*   FindChild   ( const long id );
    void        UpdateData  ();

public:

    /// Constructors
    SunDlg( );
    SunDlg( wxWindow* parent, wxWindowID id = SYMBOL_SUNDLG_IDNAME, const wxString& caption = SYMBOL_SUNDLG_TITLE, const wxPoint& pos = SYMBOL_SUNDLG_POSITION, const wxSize& size = SYMBOL_SUNDLG_SIZE, long style = SYMBOL_SUNDLG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SUNDLG_IDNAME, const wxString& caption = SYMBOL_SUNDLG_TITLE, const wxPoint& pos = SYMBOL_SUNDLG_POSITION, const wxSize& size = SYMBOL_SUNDLG_SIZE, long style = SYMBOL_SUNDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin SunDlg event handler declarations

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_POSITION
    void OnSliderPositionUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
    void OnButtonCloseClick( wxCommandEvent& event );

////@end SunDlg event handler declarations

////@begin SunDlg member function declarations

    int GetPosition() const { return m_position ; }
    void SetPosition(int value) { m_position = value ; }

////@end SunDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

};

#endif
    // _SUNDLG_H_
