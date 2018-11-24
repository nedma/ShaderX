/////////////////////////////////////////////////////////////////////////////
// Name:        AboutDlg.hpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:10:51
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _ABOUTDLG_H_
#define _ABOUTDLG_H_

#ifdef __GNUG__
#pragma interface "AboutDlg.cpp"
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
#define ID_DIALOG_ABOUT 10000
#define SYMBOL_ABOUTDLG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_ABOUTDLG_TITLE _("About")
#define SYMBOL_ABOUTDLG_IDNAME ID_DIALOG_ABOUT
#define SYMBOL_ABOUTDLG_SIZE wxSize(400, 300)
#define SYMBOL_ABOUTDLG_POSITION wxPoint(0, 0)
#define ID_BUTTON_CLOSE 10001
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * AboutDlg class declaration
 */

class AboutDlg: public wxDialog
{    

    DECLARE_CLASS( AboutDlg )
    DECLARE_EVENT_TABLE()

private:

////@begin AboutDlg member variables
////@end AboutDlg member variables

public:

    /// Constructors
    AboutDlg( );
    AboutDlg( wxWindow* parent, wxWindowID id = SYMBOL_ABOUTDLG_IDNAME, const wxString& caption = SYMBOL_ABOUTDLG_TITLE, const wxPoint& pos = SYMBOL_ABOUTDLG_POSITION, const wxSize& size = SYMBOL_ABOUTDLG_SIZE, long style = SYMBOL_ABOUTDLG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_ABOUTDLG_IDNAME, const wxString& caption = SYMBOL_ABOUTDLG_TITLE, const wxPoint& pos = SYMBOL_ABOUTDLG_POSITION, const wxSize& size = SYMBOL_ABOUTDLG_SIZE, long style = SYMBOL_ABOUTDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin AboutDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
    void OnButtonCloseClick( wxCommandEvent& event );

////@end AboutDlg event handler declarations

////@begin AboutDlg member function declarations

////@end AboutDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

};

#endif
    // _ABOUTDLG_H_
