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

#include        "TerrainRenderer.hpp"
#include        "SharedCgContext.hpp"
#include        "../logging/Logger.hpp"
#include        "../scene/StaticTerrain.hpp"
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
char TerrainRenderer::s_vertexShaderSource[] =

"   struct app2vert                                     \n"
"   {                                                   \n"
"       float4 Position : POSITION;                     \n"
"       float3 TexCoord : TEXCOORD0;                    \n"
"   };                                                  \n"
"                                                       \n"
"   struct vert2frag                                    \n"
"   {                                                   \n"
"       float4 Position     : POSITION;                 \n"
"       float2 TerrainCoord : TEXCOORD0;                \n"
"       float2 NormalCoord  : TEXCOORD1;                \n"
"   };                                                  \n"
"                                                       \n"
"   vert2frag BasicTerrainShader( app2vert          IN  \n"
"                               , uniform float4x4  MVP \n"
"                               )                       \n"
"   {                                                   \n"
"       vert2frag OUT;                                  \n"
"                                                       \n"
"       OUT.Position     = mul( MVP, IN.Position );     \n"
"       OUT.TerrainCoord = IN.TexCoord.xy;              \n"
"       OUT.NormalCoord  = IN.TexCoord.xy;              \n"
"                                                       \n"
"       return OUT;                                     \n"
"   }                                                   \n"

;

//-----------------------------------------------------------------------------
//  s_fragmentShaderSource
//-----------------------------------------------------------------------------
char TerrainRenderer::s_fragmentShaderSource[] =

"   struct vert2frag                                                            \n"
"   {                                                                           \n"
"       float4 Position     : POSITION;                                         \n"
"       float2 TerrainCoord : TEXCOORD0;                                        \n"
"       float2 NormalCoord  : TEXCOORD1;                                        \n"
"   };                                                                          \n"
"                                                                               \n"
"   float3 BasicTerrainShader( vert2frag         IN                             \n"
"                            , uniform float3    LightVec                       \n"
"                            , uniform sampler2D TerrainMap                     \n"
"                            , uniform sampler2D NormalMap                      \n"
"                            ) : COLOR                                          \n"
"   {                                                                           \n"
"       float3 normal = 2.0 * (tex2D( NormalMap, IN.NormalCoord ).xyz - 0.5);   \n"
"       float3 light  = saturate( dot( normal, LightVec ) );                    \n"
"       float3 albedo = tex2D( TerrainMap, IN.TerrainCoord ).xyz;               \n"
"                                                                               \n"
"       return light * albedo;                                                  \n"
"   }                                                                           \n"

;

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
TerrainRenderer::TerrainRenderer( const StaticTerrain* a_pTerrain
                                , const SkyDome* a_pSkyDome
                                , const unsigned char* a_terrainMap
                                , const int a_terrainMapWidth
                                , const int a_terrainMapHeight
                                , const unsigned char* a_normalMap
                                , const int a_normalMapWidth
                                , const int a_normalMapHeight
                                )
: m_pTerrain( a_pTerrain )
, m_pSkyDome( a_pSkyDome )
{
    // Create terrain textures

    glGenTextures   ( 1, &m_terrainMap );
    glBindTexture   ( GL_TEXTURE_2D, m_terrainMap );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D    ( GL_TEXTURE_2D, 0, 3, a_terrainMapWidth, a_terrainMapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, a_terrainMap );

    glGenTextures   ( 1, &m_normalMap );
    glBindTexture   ( GL_TEXTURE_2D, m_normalMap );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D    ( GL_TEXTURE_2D, 0, 3, a_normalMapWidth, a_normalMapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, a_normalMap );

    // Create basic terrain shader

    m_cgVertexShader    = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_vertexShaderSource, SharedCgContext::getInstance().getVertexProfile(), "BasicTerrainShader", NULL );
    m_cgFragmentShader  = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_fragmentShaderSource, SharedCgContext::getInstance().getFragmentProfile(), "BasicTerrainShader", NULL );

    m_shaderValid = true;

    cgGLLoadProgram( m_cgVertexShader );    if( CG_NO_ERROR != cgGetError() ) m_shaderValid = false;
    cgGLLoadProgram( m_cgFragmentShader );  if( CG_NO_ERROR != cgGetError() ) m_shaderValid = false;

    m_cgPosition    = cgGetNamedParameter( m_cgVertexShader, "IN.Position" );
    m_cgTexCoord    = cgGetNamedParameter( m_cgVertexShader, "IN.TexCoord" );
    m_cgMVP         = cgGetNamedParameter( m_cgVertexShader, "MVP" );

    m_cgLightVector = cgGetNamedParameter( m_cgFragmentShader, "LightVec" );
    m_cgTerrainMap  = cgGetNamedParameter( m_cgFragmentShader, "TerrainMap" );
    m_cgNormalMap   = cgGetNamedParameter( m_cgFragmentShader, "NormalMap" );

    // Log success

    if( true == m_shaderValid )
        Logger::getInstance().logInfo( "TerrainRenderer", "TerrainRenderer", "Default shader created successfully." );
    else
        Logger::getInstance().logError( "TerrainRenderer", "TerrainRenderer", "Default shader creation failed." );
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
TerrainRenderer::~TerrainRenderer()
{
    glDeleteTextures( 1, &m_terrainMap );
    glDeleteTextures( 1, &m_normalMap );
}

//-----------------------------------------------------------------------------
//  Render
//-----------------------------------------------------------------------------
void TerrainRenderer::render( const Camera& a_camera, const bool a_wireframe )
{
    if( NULL == m_pTerrain )
        return;

    if( true == a_wireframe )
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glPushMatrix();
    glTranslated( -m_pTerrain->getTotalWidth() * 0.5, -m_pTerrain->getTotalHeight() * 0.5, 0.0 );

    if( NULL != m_pSkyDome && NULL != m_pSkyDome->getScatterClass() && true == m_pSkyDome->getScatterClass()->isAerialPerspectiveValid() )
    {
        // Render using advanced version

        m_pSkyDome->getScatterClass()->renderAerialPerspective  ( a_camera
                                                                , m_pSkyDome->getSunDirection()
                                                                , (const float*)m_pTerrain->getVertices()
                                                                , (const float*)m_pTerrain->getTexCoords()
                                                                , m_pTerrain->getIndices()
                                                                , m_pTerrain->getIndexCount()
                                                                , m_terrainMap
                                                                , m_normalMap
                                                                );
    }
    else if( true == m_shaderValid )
    {
        // Render using basic version

        cgGLEnableClientState       ( m_cgPosition );
        cgGLEnableClientState       ( m_cgTexCoord );

        cgGLSetParameterPointer     ( m_cgPosition, 3, GL_FLOAT, 0, (void*)m_pTerrain->getVertices() );
        cgGLSetParameterPointer     ( m_cgTexCoord, 3, GL_FLOAT, 0, (void*)m_pTerrain->getTexCoords() );

        cgGLSetTextureParameter     ( m_cgTerrainMap, m_terrainMap );
        cgGLSetTextureParameter     ( m_cgNormalMap, m_normalMap );

        cgGLEnableTextureParameter  ( m_cgTerrainMap );
        cgGLEnableTextureParameter  ( m_cgNormalMap );

        if( NULL == m_pSkyDome )
            cgGLSetParameter3f( m_cgLightVector, 0.0f, 0.0f, 1.0f );
        else
            cgGLSetParameter3fv( m_cgLightVector, m_pSkyDome->getSunDirection().GetCoordinates() );

        cgGLSetStateMatrixParameter ( m_cgMVP, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY );

        cgGLEnableProfile           ( SharedCgContext::getInstance().getVertexProfile() );
        cgGLEnableProfile           ( SharedCgContext::getInstance().getFragmentProfile() );

        cgGLBindProgram             ( m_cgVertexShader );
        cgGLBindProgram             ( m_cgFragmentShader );

        glDrawElements              ( GL_TRIANGLE_STRIP, m_pTerrain->getIndexCount(), GL_UNSIGNED_INT, m_pTerrain->getIndices() );

        cgGLDisableProfile          ( SharedCgContext::getInstance().getVertexProfile() );
        cgGLDisableProfile          ( SharedCgContext::getInstance().getFragmentProfile() );

        cgGLDisableTextureParameter ( m_cgTerrainMap );
        cgGLDisableTextureParameter ( m_cgNormalMap );

        cgGLDisableClientState      ( m_cgPosition );
    }

    glPopMatrix();
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}
