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

#include        "AppFrame.hpp"
#include        "RenderView.hpp"
#include        "AboutDlg.hpp"
#include        "LogView.hpp"
#include        "RenderingDlg.hpp"
#include        "HoffmanDlg.hpp"
#include        "PreethamDlg.hpp"
#include        "SunDlg.hpp"
#include        "../logging/Logger.hpp"
#include        "../logging/wxTextCtrlHandler.hpp"
#include        "../scene/HeightField.hpp"
#include        "../render/SharedCgContext.hpp"
#include        "../render/SkyRenderer.hpp"
#include        "../render/TerrainRenderer.hpp"
#include        "../render/CompassRenderer.hpp"
#include        "../render/TextRenderer.hpp"
#include        <wx/image.h>
#include        <sstream>

using Logging::Logger;

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Event table
//*********************************************************************************************************************
//*********************************************************************************************************************

BEGIN_EVENT_TABLE(AppFrame, wxFrame)
    EVT_MENU                (EventID_Quit, AppFrame::OnQuit)
    EVT_MENU                (EventID_AboutDlg, AppFrame::OnAboutDlg)
    EVT_MENU                (EventID_RenderingDlg, AppFrame::OnRenderingDlg)
    EVT_MENU                (EventID_HoffmanItem, AppFrame::OnHoffmanItem)
    EVT_MENU                (EventID_PreethamItem, AppFrame::OnPreethamItem)
    EVT_MENU                (EventID_ScatterOptions, AppFrame::OnScatterOptions)
    EVT_MENU                (EventID_LogView, AppFrame::OnLogView)
    EVT_MENU                (EventID_SunDlg, AppFrame::OnSunDlg)
    EVT_MY_RENDERING_CHANGED(-1, AppFrame::OnRenderingChanged)
    EVT_MY_HOFFMAN_CHANGED  (-1, AppFrame::OnHoffmanChanged)
    EVT_MY_PREETHAM_CHANGED (-1, AppFrame::OnPreethamChanged)
    EVT_MY_SUN_CHANGED      (-1, AppFrame::OnSunChanged)
END_EVENT_TABLE()

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------------------
AppFrame::AppFrame(const wxString& a_title, const wxPoint& a_pos, const wxSize& a_size)
: wxFrame( static_cast<wxFrame*>(NULL), -1, a_title, a_pos, a_size )
{
    // Create "File" menu

    wxMenu* pMenuFile = new wxMenu;

    pMenuFile->Append( EventID_AboutDlg, "About..." );

    pMenuFile->AppendSeparator();

    pMenuFile->Append( EventID_Quit, "Exit" );

    // Create "Scatter" menu

    wxMenu* pScatter = new wxMenu;

    pScatter->AppendRadioItem( EventID_HoffmanItem, "Hoffman" );
    pScatter->AppendRadioItem( EventID_PreethamItem, "Preetham" );

//  pScatter->FindItemByPosition( 0 )->Check();
    pScatter->FindItemByPosition( 1 )->Check();

    // Create "Options" menu

    wxMenu* pMenuOptions = new wxMenu;

    pMenuOptions->Append( EventID_SunDlg, "Sun..." );
    pMenuOptions->Append( EventID_RenderingDlg, "Rendering..." );

    pMenuOptions->AppendSeparator();

    pMenuOptions->Append( 0, "Scattering method", pScatter );
    pMenuOptions->Append( EventID_ScatterOptions, "Scattering..." );

    pMenuOptions->AppendSeparator();

    pMenuOptions->Append( EventID_LogView, "Log..." );

    // Create menu bar

    wxMenuBar* pMenuBar = new wxMenuBar;

    pMenuBar->Append( pMenuFile, "&File" );
    pMenuBar->Append( pMenuOptions, "&Options" );

    SetMenuBar( pMenuBar );

    // Create dialogs

    m_pAboutDlg     = new AboutDlg( this );
    m_pRenderingDlg = new RenderingDlg( this );
    m_pHoffmanDlg   = new HoffmanDlg( this );
    m_pPreethamDlg  = new PreethamDlg( this );
    m_pLogView      = new LogView( this );
    m_pSunDlg       = new SunDlg( this );

    // Create canvas

    m_pRenderView = new RenderView( this );

    // Expand logging framework

    Logging::wxTextCtrlHandler textCtrlHandler( m_pLogView->GetTextCtrl() );
                               textCtrlHandler.setLevel( Logging::DEBUG );

    Logger::getInstance().addHandler( textCtrlHandler );
    Logger::getInstance().logInfo( "Logging framework initialized." );
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
AppFrame::~AppFrame()
{
    Logger::getInstance().logInfo( "Destroying logging framework..." );

    SharedCgContext::destroy();
    Logger::destroy();
}

//-----------------------------------------------------------------------------
//  initOpenGLDeps
//-----------------------------------------------------------------------------
const bool AppFrame::initOpenGLDeps()
{
    // Log used renderer

    std::ostringstream osRenderer;
    osRenderer << "Used OpenGL renderer is " << glGetString( GL_RENDERER );
    Logger::getInstance().logDebug( osRenderer.str() );

    // Check Cg profiles

    if( CG_PROFILE_UNKNOWN == SharedCgContext::getInstance().getVertexProfile() )
    {
        Logger::getInstance().logFatal( "No valid Cg vertex profile found. Aborting." );
        wxMessageDialog message( this, "No vertex program support found. Consult the log file for details. Aborting application.", "Fatal error", wxOK | wxICON_ERROR | wxSTAY_ON_TOP, wxDefaultPosition );
        message.ShowModal();
        return false;
    }

    if( CG_PROFILE_UNKNOWN == SharedCgContext::getInstance().getFragmentProfile() )
    {
        Logger::getInstance().logFatal( "No valid Cg fragment profile found. Aborting." );
        wxMessageDialog message( this, "No fragment program support found. Consult the log file for details. Aborting application.", "Fatal error", wxOK | wxICON_ERROR | wxSTAY_ON_TOP, wxDefaultPosition );
        message.ShowModal();
        return false;
    }

    // Log used Cg profiles

    std::ostringstream osVertex;
    osVertex << "Used vertex profile is " << cgGetProfileString( SharedCgContext::getInstance().getVertexProfile() );
    Logger::getInstance().logDebug( osVertex.str() );

    std::ostringstream osFragment;
    osFragment << "Used fragment profile is " << cgGetProfileString( SharedCgContext::getInstance().getFragmentProfile() );
    Logger::getInstance().logDebug( osFragment.str() );

    // Create sky dome

    m_hoffmanScatter .create();
    m_preethamScatter.create();

//  m_sky.setScatterClass   ( &m_hoffmanScatter );
    m_sky.setScatterClass   ( &m_preethamScatter );

//  m_pCurrentScatteringOptions = m_pHoffmanDlg;
    m_pCurrentScatteringOptions = m_pPreethamDlg;

    if( false == m_hoffmanScatter.isSkyLightValid() )
    {
        Logger::getInstance().logInfo( "Hoffman skylight shader is invalid. Using default sky dome shader." );
        wxMessageDialog message( this, "Hoffman skylight shader is invalid. Using default sky dome shader.", "Error", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP, wxDefaultPosition );
        message.ShowModal();
    }

    if( false == m_hoffmanScatter.isAerialPerspectiveValid() )
    {
        Logger::getInstance().logInfo( "Hoffman aerial perspective shader is invalid. Using default terrain shader." );
        wxMessageDialog message( this, "Hoffman aerial perspective shader is invalid. Using default terrain shader.", "Error", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP, wxDefaultPosition );
        message.ShowModal();
    }

    if( false == m_preethamScatter.isSkyLightValid() )
    {
        Logger::getInstance().logInfo( "Preetham skylight shader is invalid. Using default sky dome shader." );
        wxMessageDialog message( this, "Preetham skylight shader is invalid. Using default sky dome shader.", "Error", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP, wxDefaultPosition );
        message.ShowModal();
    }

    if( false == m_preethamScatter.isAerialPerspectiveValid() )
    {
        Logger::getInstance().logInfo( "Preetham aerial perspective shader is invalid. Using default terrain shader." );
        wxMessageDialog message( this, "Preetham aerial perspective shader is invalid. Using default terrain shader.", "Error", wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP, wxDefaultPosition );
        message.ShowModal();
    }

    // Create a new sky renderer

    SkyRenderer* pSky = new SkyRenderer( &m_sky );

    m_pRenderView->AddRenderer( pSky );

    Logger::getInstance().logInfo( "Sky dome and scatter classes initialized." );

    // Create a new terrain height field and landscape

    wxImage heightMap;
            heightMap.LoadFile( "heights.bmp", wxBITMAP_TYPE_BMP );

    HeightField heightField;
                heightField.createFromRGBImage  ( heightMap.GetData()
                                                , heightMap.GetWidth()
                                                , heightMap.GetHeight()
                                                , 1.0
                                                , 20.0
                                                );

    m_terrain.create( heightField );

    // Create a new terrain renderer

    wxImage terrainMap;
            terrainMap.LoadFile( "terrain.bmp", wxBITMAP_TYPE_BMP );

    wxImage normalMap;
            normalMap.LoadFile( "normals.bmp", wxBITMAP_TYPE_BMP );

    TerrainRenderer* pTerrain = new TerrainRenderer( &m_terrain
                                                   , &m_sky
                                                   , terrainMap.GetData()
                                                   , terrainMap.GetWidth(), terrainMap.GetHeight()
                                                   , normalMap.GetData()
                                                   , normalMap.GetWidth(), normalMap.GetHeight()
                                                   );

    m_pRenderView->AddRenderer( pTerrain );

    Logger::getInstance().logInfo( "Terrain initialized." );

    // Create a new text renderer

    wxImage fontTexture;
            fontTexture.LoadFile( "font.bmp", wxBITMAP_TYPE_BMP );

    TextRenderer* pText = new TextRenderer( fontTexture.GetData(), fontTexture.GetWidth(), fontTexture.GetHeight() );

    m_pRenderView->AddRenderer( pText );

    Logger::getInstance().logInfo( "Text renderer initialized." );

    // Create a new compass renderer

    wxImage compassTexture;
            compassTexture.LoadFile( "compass.bmp", wxBITMAP_TYPE_BMP );

    CompassRenderer* pCompass = new CompassRenderer( compassTexture.GetData(), compassTexture.GetWidth(), compassTexture.GetHeight() );

    m_pRenderView->AddRenderer( pCompass );

    Logger::getInstance().logInfo( "Compass renderer initialized." );

    return true;
}

//-----------------------------------------------------------------------------
//  OnQuit
//-----------------------------------------------------------------------------
void AppFrame::OnQuit(wxCommandEvent& WXUNUSED(a_Event))
{
    Close( true );
}

//-----------------------------------------------------------------------------
//  OnAboutDlg
//-----------------------------------------------------------------------------
void AppFrame::OnAboutDlg(wxCommandEvent& WXUNUSED(a_Event))
{
    m_pAboutDlg->ShowModal();
}

//-----------------------------------------------------------------------------
//  OnRenderingDlg
//-----------------------------------------------------------------------------
void AppFrame::OnRenderingDlg(wxCommandEvent& WXUNUSED(a_Event))
{
    m_pRenderingDlg->Show();
}

//-----------------------------------------------------------------------------
//  OnHoffmanItem
//-----------------------------------------------------------------------------
void AppFrame::OnHoffmanItem(wxCommandEvent& WXUNUSED(a_Event))
{
    m_sky.setScatterClass( &m_hoffmanScatter );
    m_pCurrentScatteringOptions->Show( false );
    m_pCurrentScatteringOptions = m_pHoffmanDlg;
    m_pRenderView->Refresh();
}

//-----------------------------------------------------------------------------
//  OnPreethamItem
//-----------------------------------------------------------------------------
void AppFrame::OnPreethamItem(wxCommandEvent& WXUNUSED(a_Event))
{
    m_sky.setScatterClass( &m_preethamScatter );
    m_pCurrentScatteringOptions->Show( false );
    m_pCurrentScatteringOptions = m_pPreethamDlg;
    m_pRenderView->Refresh();
}

//-----------------------------------------------------------------------------
//  OnScatterOptions
//-----------------------------------------------------------------------------
void AppFrame::OnScatterOptions(wxCommandEvent& WXUNUSED(a_Event))
{
    m_pCurrentScatteringOptions->Show();
}

//-----------------------------------------------------------------------------
//  OnLogView
//-----------------------------------------------------------------------------
void AppFrame::OnLogView(wxCommandEvent& WXUNUSED(a_Event))
{
    m_pLogView->Show();
}

//-----------------------------------------------------------------------------
//  OnSunDlg
//-----------------------------------------------------------------------------
void AppFrame::OnSunDlg(wxCommandEvent& WXUNUSED(a_Event))
{
    m_pSunDlg->Show();
}

//-----------------------------------------------------------------------------
//  OnRenderingChanged
//-----------------------------------------------------------------------------
void AppFrame::OnRenderingChanged(wxCommandEvent& WXUNUSED(a_Event))
{
    // Get data from dialog

    if( NULL == m_pRenderingDlg )
        return;

    // Update sky dome

    m_sky.create( 5160.0, 1000.0, m_pRenderingDlg->GetRings(), m_pRenderingDlg->GetSegments() );

    // Update render view

    m_pRenderView->SetWireframe( m_pRenderingDlg->GetWireframe() );

    // Redraw render view

    m_pRenderView->Refresh();
}

//-----------------------------------------------------------------------------
//  OnHoffmanChanged
//-----------------------------------------------------------------------------
void AppFrame::OnHoffmanChanged(wxCommandEvent& WXUNUSED(a_Event))
{
    // Get data from dialog

    if( NULL == m_pHoffmanDlg )
        return;

    // Update Hoffman scatter class

    m_hoffmanScatter.setSunIntensity( m_pHoffmanDlg->GetIntensity() );
    m_hoffmanScatter.setMie         ( m_pHoffmanDlg->GetMie() );
    m_hoffmanScatter.setRayleigh    ( m_pHoffmanDlg->GetRayleigh() );
    m_hoffmanScatter.setHg          ( m_pHoffmanDlg->GetHGg() );
    m_hoffmanScatter.setInscattering( m_pHoffmanDlg->GetInscattering() );
    m_hoffmanScatter.setTurbidity   ( m_pHoffmanDlg->GetTurbidity() );

    // Redraw render view

    m_pRenderView->Refresh();
}

//-----------------------------------------------------------------------------
//  OnPreethamChanged
//-----------------------------------------------------------------------------
void AppFrame::OnPreethamChanged(wxCommandEvent& WXUNUSED(a_Event))
{
    // Get data from dialog

    if( NULL == m_pPreethamDlg )
        return;

    // Update Preetham scatter class

    m_preethamScatter.setTurbidity      ( m_pPreethamDlg->GetTurbidity() );
    m_preethamScatter.setScaleFactors   ( m_pPreethamDlg->GetExposure(), m_pPreethamDlg->GetScaleInscatter() );

    // Redraw render view

    m_pRenderView->Refresh();
}

//-----------------------------------------------------------------------------
//  OnSunChanged
//-----------------------------------------------------------------------------
void AppFrame::OnSunChanged(wxCommandEvent& WXUNUSED(a_Event))
{
    // Get data from dialog

    if( NULL == m_pSunDlg )
        return;

    // Update sun direction

    m_sky.setSunDirection( CVector3 ( cos( m_pSunDlg->GetPosition() * g_dDegToRad )
                                    , 0.0f
                                    , sin( m_pSunDlg->GetPosition() * g_dDegToRad )
                                    )
                         );

    // Redraw render view

    m_pRenderView->Refresh();
}
