//*****************************************************************************
//
//  Copyright (c) 2004 Marco Spoerl
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//*****************************************************************************

#ifndef         APPFRAME_H_INCLUDED
#define         APPFRAME_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <wx/wx.h>
#include        "../scene/StaticTerrain.hpp"
#include        "../scene/SkyDome.hpp"
#include        "../scatter/HoffmanScatter.hpp"
#include        "../scatter/PreethamScatter.hpp"

class           RenderView;
class           AboutDlg;
class           DateTimeDlg;
class           LocationDlg;
class           RenderingDlg;
class           HoffmanDlg;
class           PreethamDlg;
class           LogView;
class           SunDlg;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  AppFrame
//-----------------------------------------------------------------------------
//
//! The sky application frame
//
//-----------------------------------------------------------------------------
class AppFrame : public wxFrame
{

public:

    //
    // Event Ids
    //

    enum EventID
    {
        EventID_Quit        = 1 ,
        EventID_AboutDlg        ,
        EventID_RenderingDlg    ,
        EventID_HoffmanItem     ,
        EventID_PreethamItem    ,
        EventID_ScatterOptions  ,
        EventID_LogView         ,
        EventID_SunDlg
    };

    //
    // Construction
    //

    //! \brief  Constructs a new AppFrame
    //! \param  [in] Window caption
    //! \param  [in] Window position
    //! \param  [in] Window size
                AppFrame            ( const wxString& a_title, const wxPoint& a_pos, const wxSize& a_size );
    //! \brief  Destructor
                ~AppFrame           ();

    //! \brief  Initializes the OpenGL dependencies
    const bool  initOpenGLDeps      ();

    //
    // Event callbacks
    //

    void        OnQuit              ( wxCommandEvent& a_Event );
    void        OnAboutDlg          ( wxCommandEvent& a_Event );
    void        OnRenderingDlg      ( wxCommandEvent& a_Event );
    void        OnHoffmanItem       ( wxCommandEvent& a_Event );
    void        OnPreethamItem      ( wxCommandEvent& a_Event );
    void        OnScatterOptions    ( wxCommandEvent& a_Event );
    void        OnLogView           ( wxCommandEvent& a_Event );
    void        OnSunDlg            ( wxCommandEvent& a_Event );
    void        OnDateTimeChanged   ( wxCommandEvent& a_Event );
    void        OnLocationChanged   ( wxCommandEvent& a_Event );
    void        OnRenderingChanged  ( wxCommandEvent& a_Event );
    void        OnHoffmanChanged    ( wxCommandEvent& a_Event );
    void        OnPreethamChanged   ( wxCommandEvent& a_Event );
    void        OnSunChanged        ( wxCommandEvent& a_Event );

    DECLARE_EVENT_TABLE()

protected:

    //
    // Custom wxGLcanvas control
    //

    RenderView*     m_pRenderView; //!< Render view custom widget instance

    //
    // Dialogs
    //

    AboutDlg*       m_pAboutDlg;     //!< About dialog instance
    RenderingDlg*   m_pRenderingDlg; //!< Rendering options dialog instance
    HoffmanDlg*     m_pHoffmanDlg;   //!< Hoffman scatter options dialog instance
    PreethamDlg*    m_pPreethamDlg;  //!< Preetham scatter options dialog instance
    LogView*        m_pLogView;      //!< Log view instance
    SunDlg*         m_pSunDlg;       //!< Sun options dialog instance

    wxDialog*       m_pCurrentScatteringOptions; //!< Pointer to the current scatter class' options

    //
    // Scene elements
    //

    StaticTerrain   m_terrain;          //!< Simple static landscape
    SkyDome         m_sky;              //!< The sky dome
    HoffmanScatter  m_hoffmanScatter;   //!< Hoffman scatter instance
    PreethamScatter m_preethamScatter;  //!< Preetham scatter instance

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a AppFrame from the specified one
    //! \param  [in] The AppFrame to copy from
                AppFrame            ( const AppFrame& );
    //! \brief  Sets this AppFrame to the specified one
    //! \param  [in] The AppFrame to copy from
    //! \return The modified AppFrame
    AppFrame&   operator=           ( const AppFrame& );

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // APPFRAME_H_INCLUDED
