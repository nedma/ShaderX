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

#include        "CompassRenderer.hpp"
#include        "Camera.hpp"

#ifdef          WIN32
#include        <windows.h>
#endif

#include        <gl/gl.h>
#include        <gl/glu.h>

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
CompassRenderer::CompassRenderer( const unsigned char* a_pTexture, const int a_width, const int a_height )
: m_texture( -1 )
{
    // Create compass texture

    glGenTextures   ( 1, &m_texture );
    glBindTexture   ( GL_TEXTURE_2D, m_texture );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D    ( GL_TEXTURE_2D, 0, 3, a_width, a_height, 0, GL_RGB, GL_UNSIGNED_BYTE, a_pTexture );
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
CompassRenderer::~CompassRenderer()
{
    glDeleteTextures( 1, &m_texture );
}

//-----------------------------------------------------------------------------
//  render
//-----------------------------------------------------------------------------
void CompassRenderer::render( const Camera& a_camera, const bool a_wireframe )
{
    // Setup ortho projection matrix

    glMatrixMode    ( GL_PROJECTION );
    glPushMatrix    ();
    glLoadIdentity  ();
    gluOrtho2D      ( 0.0, static_cast<GLdouble>(a_camera.getViewportWidth()), 0.0, static_cast<GLdouble>(a_camera.getViewportHeight()) );

    // Setup modelview matrix

    glMatrixMode    ( GL_MODELVIEW );
    glPushMatrix    ();
    glLoadIdentity  ();

    // Determine rotation angle

    CVector3    vecView ( a_camera.getView().GetX(), a_camera.getView().GetY(), 0.0f );
                vecView.Normalize();
    CVector3    vecNorth( 0.0f, 1.0f, 0.0f );

    float   fDot    = CVector3::DotProduct( vecNorth, vecView );
    float   fAngle  = g_acosf( fDot ) * g_fRadToDeg;
    float   fAxis   = vecView.GetX() < vecNorth.GetX() ? -1.0f : 1.0f;

    // Draw textured quad

    glEnable        ( GL_TEXTURE_2D );
    glBindTexture   ( GL_TEXTURE_2D, m_texture );
    glEnable        ( GL_BLEND );
    glBlendFunc     ( GL_SRC_ALPHA, GL_ONE );
    glDisable       ( GL_DEPTH_TEST );
    glDepthMask     ( false );
    glTranslatef    ( 70.0f, 70.0f, 0.0f );
    glRotatef       ( fAngle, 0.0f, 0.0f, fAxis );
    glTranslatef    ( -70.0f, -70.0f, 0.0f );
    glColor3f       ( 1.0f, 1.0f, 1.0f );
    glBegin         ( GL_QUADS );
        glTexCoord2f( 0.0f, 1.0f );glVertex2i   ( 0, 0 );
        glTexCoord2f( 1.0f, 1.0f );glVertex2i   ( 140, 0 );
        glTexCoord2f( 1.0f, 0.0f );glVertex2i   ( 140, 140 );
        glTexCoord2f( 0.0f, 0.0f );glVertex2i   ( 0, 140 );
    glEnd           ();
    glDisable       ( GL_TEXTURE_2D );
    glDisable       ( GL_BLEND );
    glEnable        ( GL_DEPTH_TEST );
    glDepthMask     ( true );

    // Restore old matrices

    glMatrixMode    ( GL_PROJECTION );
    glPopMatrix     ();

    glMatrixMode    ( GL_MODELVIEW );
    glPopMatrix     ();
}
