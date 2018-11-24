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

#include        "RenderView.hpp"
#include        <gl/glu.h>
#include        <algorithm>
#include        <functional>

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Functors
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  RenderObject
//-----------------------------------------------------------------------------
class RenderObject : public std::unary_function< IRenderer, void >
{

private:

    const Camera&   m_camera;
    const bool      m_wireframe;

public:

    RenderObject(const Camera& a_camera, const bool a_wireframe)
        : m_camera( a_camera ), m_wireframe( a_wireframe )
    {}

    void operator() ( IRenderer* a_pRenderer )
    {
        a_pRenderer->render( m_camera, m_wireframe );
    }

};

//-----------------------------------------------------------------------------
//  DeleteObject
//-----------------------------------------------------------------------------
struct DeleteObject
{

    template< typename T >
    void operator() ( T* a_pObject ) const
    {
        delete a_pObject;
    }

};

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Event table
//*********************************************************************************************************************
//*********************************************************************************************************************

BEGIN_EVENT_TABLE(RenderView, wxGLCanvas)
    EVT_LEFT_DOWN       (RenderView::OnClick)
    EVT_ERASE_BACKGROUND(RenderView::OnEraseBackground)
    EVT_MOTION          (RenderView::OnMotion)
    EVT_PAINT           (RenderView::OnPaint)
    EVT_SIZE            (RenderView::OnSize)
END_EVENT_TABLE()

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Static initialization
//*********************************************************************************************************************
//*********************************************************************************************************************

int RenderView::ms_iAttributes[] =
{
    WX_GL_RGBA          ,
    WX_GL_LEVEL         , 0 ,
    WX_GL_DOUBLEBUFFER  ,
    WX_GL_MIN_RED       , 8 ,
    WX_GL_MIN_GREEN     , 8 ,
    WX_GL_MIN_BLUE      , 8 ,
//  WX_GL_MIN_ALPHA     , 8 ,
//  WX_GL_DEPTH_SIZE    , 32,
    WX_GL_STENCIL_SIZE  , 0 ,
    0
};

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------------------
RenderView::RenderView( wxWindow* a_pParent )
: wxGLCanvas( a_pParent, -1, wxDefaultPosition, wxDefaultSize, 0, _T("RenderView") )
, m_wireframe( false )
, m_lastX( 0 )
, m_lastY( 0 )
, m_mouseSensitivity( 200.0f )
, m_maxAngle( 1.5f )
, m_currentRotationAboutX( 0.0f )
{
    // Initialize OpenGL

    SetCurrent();

    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    glEnable    ( GL_CULL_FACE );
    glEnable    ( GL_DEPTH_TEST );
    
    glCullFace  ( GL_BACK );
    glShadeModel( GL_SMOOTH );

    // Initialize camera

    m_camera.setPosition( CVector3( 0.0f, 0.0f, 24.0f ) );
    m_camera.setUp      ( CVector3( 0.0f, 0.0f, 1.0f ) );
    m_camera.setView    ( CVector3( 0.0f, 1.0f, 0.0f ) );
    m_camera.setFarClip ( 6000.0f );
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
RenderView::~RenderView()
{
    SetCurrent();
    std::for_each( m_renderer.begin(), m_renderer.end(), DeleteObject );
}

//-----------------------------------------------------------------------------
//  OnClick
//-----------------------------------------------------------------------------
void RenderView::OnClick( wxMouseEvent& a_event )
{
    m_lastX = a_event.GetX();
    m_lastY = a_event.GetY();
}

//-----------------------------------------------------------------------------
//  OnEraseBackground
//-----------------------------------------------------------------------------
void RenderView::OnEraseBackground( wxEraseEvent& a_event )
{}

//-----------------------------------------------------------------------------
//  OnMotion
//-----------------------------------------------------------------------------
void RenderView::OnMotion( wxMouseEvent& a_event )
{
    // Only handle motions after clicks

    if( false == a_event.LeftIsDown() )
    {
        a_event.Skip();
        return;
    }

    // Rotate camera

    CVector3 vecMouseDirection;

    if( a_event.GetX() == m_lastX && a_event.GetY() == m_lastY )
        return;

    vecMouseDirection.AccessX() = (m_lastX - a_event.GetX()) / m_mouseSensitivity; 
    vecMouseDirection.AccessY() = (m_lastY - a_event.GetY()) / m_mouseSensitivity;

    m_currentRotationAboutX += vecMouseDirection.GetY();

    if( m_currentRotationAboutX > m_maxAngle )
    {
        m_currentRotationAboutX = m_maxAngle;
        return;
    }

    if( m_currentRotationAboutX < -m_maxAngle )
    {
        m_currentRotationAboutX = -m_maxAngle;
        return;
    }

    CVector3    vecAxis = CVector3::CrossProduct( m_camera.getView() - m_camera.getPosition(), m_camera.getUp() );
                vecAxis.Normalize();

    m_camera.rotate( vecMouseDirection.GetY(), vecAxis.GetX(), vecAxis.GetY(), vecAxis.GetZ() );
    m_camera.rotate( vecMouseDirection.GetX(), 0.0f, 0.0f, 1.0f );

    // Store current position

    m_lastX = a_event.GetX();
    m_lastY = a_event.GetY();

    Refresh();
}

//-----------------------------------------------------------------------------
//  OnPaint
//-----------------------------------------------------------------------------
void RenderView::OnPaint( wxPaintEvent& a_event )
{
    // Get device context and set canvas as current render target

    wxPaintDC dc( this );

    SetCurrent();

    // Set modelview matrix

    glMatrixMode    ( GL_MODELVIEW );
    glLoadIdentity  ();
    gluLookAt       ( m_camera.getPosition().GetX(), m_camera.getPosition().GetY(), m_camera.getPosition().GetZ()
                    , m_camera.getCenter().GetX(), m_camera.getCenter().GetY(), m_camera.getCenter().GetZ()
                    , m_camera.getUp().GetX(), m_camera.getUp().GetY(), m_camera.getUp().GetZ()
                    );

    // Clear

    glClear         ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Render scene

    std::for_each( m_renderer.begin(), m_renderer.end(), RenderObject( m_camera, m_wireframe ) );

    // Display scene

    glFinish        ();

    SwapBuffers();
}

//-----------------------------------------------------------------------------
//  OnSize
//-----------------------------------------------------------------------------
void RenderView::OnSize( wxSizeEvent& a_event )
{
    SetCurrent();

    // Call base class

    wxGLCanvas::OnSize( a_event );

    // Retrieve canvas size

    int iWidth, iHeight;

    GetClientSize( &iWidth, &iHeight );

    // Update camera

    m_camera.setViewportWidth   ( iWidth );
    m_camera.setViewportHeight  ( iHeight );

    // Set viewport

    glViewport      ( 0, 0, iWidth, iHeight );

    // Set projection matrix

    glMatrixMode    ( GL_PROJECTION );
    glLoadIdentity  ();
    gluPerspective  ( m_camera.getFoV()
                    , m_camera.getAspect()
                    , m_camera.getNearClip()
                    , m_camera.getFarClip()
                    );
}

//-----------------------------------------------------------------------------
//  AddRenderer
//-----------------------------------------------------------------------------
void RenderView::AddRenderer( IRenderer* a_pRenderer )
{
    m_renderer.push_back( a_pRenderer );
}

//-----------------------------------------------------------------------------
//  SetWireframe
//-----------------------------------------------------------------------------
void RenderView::SetWireframe( const bool a_wireframe )
{
    m_wireframe = a_wireframe;
}
