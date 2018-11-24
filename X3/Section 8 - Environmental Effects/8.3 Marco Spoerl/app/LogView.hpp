/////////////////////////////////////////////////////////////////////////////
// Name:        LogView.hpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:16:50
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _LOGVIEW_H_
#define _LOGVIEW_H_

#ifdef __GNUG__
#pragma interface "LogView.cpp"
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
#define ID_DIALOG_LOG 10000
#define SYMBOL_LOGVIEW_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_LOGVIEW_TITLE _("Log")
#define SYMBOL_LOGVIEW_IDNAME ID_DIALOG_LOG
#define SYMBOL_LOGVIEW_SIZE wxSize(400, 300)
#define SYMBOL_LOGVIEW_POSITION wxPoint(0, 0)
#define ID_TEXTCTRL_LOG 10002
#define ID_BUTTON_CLOSE 10001
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * LogView class declaration
 */

class LogView: public wxDialog
{    

    DECLARE_CLASS( LogView )
    DECLARE_EVENT_TABLE()

private:

////@begin LogView member variables
////@end LogView member variables

public:


    /// Constructors
    LogView( );
    LogView( wxWindow* parent, wxWindowID id = SYMBOL_LOGVIEW_IDNAME, const wxString& caption = SYMBOL_LOGVIEW_TITLE, const wxPoint& pos = SYMBOL_LOGVIEW_POSITION, const wxSize& size = SYMBOL_LOGVIEW_SIZE, long style = SYMBOL_LOGVIEW_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_LOGVIEW_IDNAME, const wxString& caption = SYMBOL_LOGVIEW_TITLE, const wxPoint& pos = SYMBOL_LOGVIEW_POSITION, const wxSize& size = SYMBOL_LOGVIEW_SIZE, long style = SYMBOL_LOGVIEW_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin LogView event handler declarations
    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
    void OnButtonCloseClick( wxCommandEvent& event );

////@end LogView event handler declarations

////@begin LogView member function declarations
////@end LogView member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    wxTextCtrl* GetTextCtrl () const;

};

#endif
    // _LOGVIEW_H_
