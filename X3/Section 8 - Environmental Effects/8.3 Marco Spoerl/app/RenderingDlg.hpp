/////////////////////////////////////////////////////////////////////////////
// Name:        RenderingDlg.hpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     05/21/04 17:16:45
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RENDERINGDLG_H_
#define _RENDERINGDLG_H_

#ifdef __GNUG__
#pragma interface "RenderingDlg.cpp"
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
#define ID_DIALOG_RENDERING 10000
#define SYMBOL_RENDERINGDLG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_RENDERINGDLG_TITLE _("Rendering")
#define SYMBOL_RENDERINGDLG_IDNAME ID_DIALOG_RENDERING
#define SYMBOL_RENDERINGDLG_SIZE wxSize(400, 300)
#define SYMBOL_RENDERINGDLG_POSITION wxPoint(0, 0)
#define ID_CHECKBOX_WIREFRAME 10002
#define ID_SLIDER_RINGS 10003
#define ID_TEXTCTRL_RINGS 10004
#define ID_SLIDER_SEGMENTS 10005
#define ID_TEXTCTRL_SEGMENTS 10006
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
    DECLARE_EVENT_TYPE(wxEVT_MY_RENDERING_CHANGED, 7777)
END_DECLARE_EVENT_TYPES()

#define EVT_MY_RENDERING_CHANGED(id, fn) \
        DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_MY_RENDERING_CHANGED, id, -1, \
        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&fn, \
        (wxObject *) NULL \
        ),

/*!
 * RenderingDlg class declaration
 */

class RenderingDlg: public wxDialog
{    

    DECLARE_CLASS( RenderingDlg )
    DECLARE_EVENT_TABLE()

private:

////@begin RenderingDlg member variables
    int m_rings;
    int m_segments;
////@end RenderingDlg member variables

    wxWindow*   FindChild   ( const long id );
    void        UpdateData  ();

public:

    /// Constructors
    RenderingDlg( );
    RenderingDlg( wxWindow* parent, wxWindowID id = SYMBOL_RENDERINGDLG_IDNAME, const wxString& caption = SYMBOL_RENDERINGDLG_TITLE, const wxPoint& pos = SYMBOL_RENDERINGDLG_POSITION, const wxSize& size = SYMBOL_RENDERINGDLG_SIZE, long style = SYMBOL_RENDERINGDLG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_RENDERINGDLG_IDNAME, const wxString& caption = SYMBOL_RENDERINGDLG_TITLE, const wxPoint& pos = SYMBOL_RENDERINGDLG_POSITION, const wxSize& size = SYMBOL_RENDERINGDLG_SIZE, long style = SYMBOL_RENDERINGDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin RenderingDlg event handler declarations
    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_WIREFRAME
    void OnCheckboxWireframeClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_RINGS
    void OnSliderRingsUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_SEGMENTS
    void OnSliderSegmentsUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CLOSE
    void OnButtonCloseClick( wxCommandEvent& event );

////@end RenderingDlg event handler declarations

////@begin RenderingDlg member function declarations
    int GetRings() const { return m_rings ; }
    void SetRings(int value) { m_rings = value ; }

    int GetSegments() const { return m_segments ; }
    void SetSegments(int value) { m_segments = value ; }

////@end RenderingDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    const bool GetWireframe() { return static_cast<wxCheckBox*>( FindChild( ID_CHECKBOX_WIREFRAME ) )->IsChecked(); }

};

#endif
    // _RENDERINGDLG_H_
