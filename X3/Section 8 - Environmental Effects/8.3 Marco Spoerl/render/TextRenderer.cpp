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

#include        "TextRenderer.hpp"
#include        "Camera.hpp"
#include        "SharedCgContext.hpp"

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
TextRenderer::TextRenderer( const unsigned char* a_pTexture, const int a_width, const int a_height )
{
    // Initialize font

    glGenTextures   ( 1, &m_texture );
    glBindTexture   ( GL_TEXTURE_2D, m_texture );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D    ( GL_TEXTURE_2D, 0, 3, a_width, a_height, 0, GL_RGB, GL_UNSIGNED_BYTE, a_pTexture );

    m_font.SetTexture( m_texture, 16, 16 );
    m_font.SetFontProperties( 16, 16, 10 );
    m_font.BuildFont();

    // Initialize text

    m_renderer          = (char*)glGetString( GL_RENDERER );
    m_version           = (char*)glGetString( GL_VERSION );
    m_vertexProfile     = cgGetProfileString( SharedCgContext::getInstance().getVertexProfile() );
    m_fragmentProfile   = cgGetProfileString( SharedCgContext::getInstance().getFragmentProfile() );
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
TextRenderer::~TextRenderer()
{
    m_font.KillFont();
    glDeleteTextures( 1, &m_texture );
}

//-----------------------------------------------------------------------------
//  render
//-----------------------------------------------------------------------------
void TextRenderer::render( const Camera& a_camera, const bool a_wireframe )
{
    glEnable    ( GL_TEXTURE_2D );
    glEnable    ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE );
    glColor3f   ( 1.0f, 1.0f, 1.0f );
    glDepthMask ( false );

    m_font.SetBase( 32 );
    m_font.SetDisplayMode( a_camera.getViewportWidth(), a_camera.getViewportHeight() );

    int         iTextLine   = 5;
    const int   iLineHeight = 15;

    iTextLine += iLineHeight; m_font.Print( 5, iTextLine, m_renderer.c_str() );
    iTextLine += iLineHeight; m_font.Print( 5, iTextLine, m_version.c_str() );
    iTextLine += iLineHeight; m_font.Print( 5, iTextLine, m_vertexProfile.c_str() );
    iTextLine += iLineHeight; m_font.Print( 5, iTextLine, m_fragmentProfile.c_str() );

    glDisable   ( GL_TEXTURE_2D );
    glDisable   ( GL_BLEND );
    glDepthMask ( true );
}
