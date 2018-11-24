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

#include        "SkyRenderer.hpp"
#include        "SharedCgContext.hpp"
#include        "../logging/Logger.hpp"
#include        "../scene/SkyDome.hpp"
#include        "../scatter/IScatter.hpp"
#include        <Cg/cgGL.h>

using Logging::Logger;

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Shader sources
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  s_vertexShaderSource
//-----------------------------------------------------------------------------
char SkyRenderer::s_vertexShaderSource[] =

"   struct app2vert                                             \n"
"   {                                                           \n"
"       float4 Position : POSITION;                             \n"
"   };                                                          \n"
"                                                               \n"
"   struct vert2frag                                            \n"
"   {                                                           \n"
"       float4 Position : POSITION;                             \n"
"   };                                                          \n"
"                                                               \n"
"   vert2frag BasicSkyShader( app2vert IN )                     \n"
"   {                                                           \n"
"       vert2frag OUT;                                          \n"
"                                                               \n"
"       OUT.Position = mul( glstate.matrix.mvp, IN.Position );  \n"
"                                                               \n"
"       return OUT;                                             \n"
"   }                                                           \n"

;

//-----------------------------------------------------------------------------
//  s_fragmentShaderSource
//-----------------------------------------------------------------------------
char SkyRenderer::s_fragmentShaderSource[] =

"   struct vert2frag                                    \n"
"   {                                                   \n"
"       float4 Position : POSITION;                     \n"
"   };                                                  \n"
"                                                       \n"
"   float3 BasicSkyShader( vert2frag IN ) : COLOR       \n"
"   {                                                   \n"
"       return float3( 0.0, 1.0, 0.0 );                 \n"
"   }                                                   \n"

;

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
SkyRenderer::SkyRenderer( const SkyDome* a_pSkyDome )
: m_pSkyDome( a_pSkyDome )
{
    // Create basic sky shader

    m_cgVertexShader   = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_vertexShaderSource, SharedCgContext::getInstance().getVertexProfile(), "BasicSkyShader", NULL );
    m_cgFragmentShader = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_fragmentShaderSource, SharedCgContext::getInstance().getFragmentProfile(), "BasicSkyShader", NULL );

    m_shaderValid = true;

    cgGLLoadProgram( m_cgVertexShader );    if( CG_NO_ERROR != cgGetError() ) m_shaderValid = false;
    cgGLLoadProgram( m_cgFragmentShader );  if( CG_NO_ERROR != cgGetError() ) m_shaderValid = false;

    m_cgPosition = cgGetNamedParameter( m_cgVertexShader, "IN.Position" );

    // Log success

    if( true == m_shaderValid )
        Logger::getInstance().logInfo( "SkyRenderer", "SkyRenderer", "Default shader created successfully." );
    else
        Logger::getInstance().logError( "SkyRenderer", "SkyRenderer", "Default shader creation failed." );
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
SkyRenderer::~SkyRenderer()
{}

//-----------------------------------------------------------------------------
//  render
//-----------------------------------------------------------------------------
void SkyRenderer::render( const Camera& a_camera, const bool a_wireframe )
{
    if( true == a_wireframe )
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if( NULL != m_pSkyDome && NULL != m_pSkyDome->getScatterClass() && true == m_pSkyDome->getScatterClass()->isSkyLightValid() )
    {
        // Render using advanced version

        m_pSkyDome->getScatterClass()->renderSkyLight( a_camera
                                                     , m_pSkyDome->getSunDirection()
                                                     , (const float*)m_pSkyDome->getVertices()
                                                     , m_pSkyDome->getIndices()
                                                     , m_pSkyDome->getIndexCount()
                                                     );
    }
    else if( true == m_shaderValid )
    {
        // Render using basic version

        glDisable               ( GL_DEPTH_TEST );
        glDepthMask             ( false );

        cgGLEnableClientState   ( m_cgPosition );

        cgGLSetParameterPointer ( m_cgPosition, 3, GL_FLOAT, 0, (void*)m_pSkyDome->getVertices() );

        cgGLEnableProfile       ( SharedCgContext::getInstance().getVertexProfile() );
        cgGLEnableProfile       ( SharedCgContext::getInstance().getFragmentProfile() );

        cgGLBindProgram         ( m_cgVertexShader );
        cgGLBindProgram         ( m_cgFragmentShader );
        
        glDrawElements          ( GL_TRIANGLE_STRIP, m_pSkyDome->getIndexCount(), GL_UNSIGNED_INT, m_pSkyDome->getIndices() );

        cgGLDisableProfile      ( SharedCgContext::getInstance().getVertexProfile() );
        cgGLDisableProfile      ( SharedCgContext::getInstance().getFragmentProfile() );

        cgGLDisableClientState  ( m_cgPosition );

        glDepthMask             ( true );
        glEnable                ( GL_DEPTH_TEST );
    }

    if( true == a_wireframe )
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}
