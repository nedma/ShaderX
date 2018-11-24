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

#include        "HoffmanScatter.hpp"
#include        "../logging/Logger.hpp"
#include        "../render/SharedCgContext.hpp"
#include        "../render/Camera.hpp"
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
char HoffmanScatter::s_vertexShaderSource[] =

"   struct app2vert                                                                         \n"
"   {                                                                                       \n"
"       float4 Position : POSITION;                                                         \n"
"       float3 TexCoord : TEXCOORD0;                                                        \n"
"   };                                                                                      \n"
"                                                                                           \n"
"   struct vert2frag                                                                        \n"
"   {                                                                                       \n"
"       float4 Position     : POSITION;                                                     \n"
"       float2 TerrainCoord : TEXCOORD0;                                                    \n"
"       float2 NormalCoord  : TEXCOORD1;                                                    \n"
"       float3 Lin          : COLOR0;                                                       \n"
"       float3 Fex          : COLOR1;                                                       \n"
"   };                                                                                      \n"
"                                                                                           \n"
"   vert2frag HoffmanShader ( app2vert IN                                                   \n"
"                           , uniform float3    EyePos                                      \n"
"                           , uniform float3    SunDir                                      \n"
"                           , uniform float3    BetaRPlusBetaM                              \n"
"                           , uniform float3    HGg                                         \n"
"                           , uniform float3    BetaDashR                                   \n"
"                           , uniform float3    BetaDashM                                   \n"
"                           , uniform float3    OneOverBetaRPlusBetaM                       \n"
"                           , uniform float4    Multipliers                                 \n"
"                           , uniform float4    SunColorAndIntensity                        \n"
"                           , uniform float4x4  MVP                                         \n"
"                           , uniform float4x4  MV                                          \n"
"                           )                                                               \n"
"   {                                                                                       \n"
"       //                                                                                  \n"
"       // Calculate view direction and distance                                            \n"
"       //                                                                                  \n"
"                                                                                           \n"
"       float3 vertex        = mul( MV, IN.Position ).xyz;                                  \n"
"       float3 viewDirection = normalize( vertex );                                         \n"
"       float  distance      = length( vertex );                                            \n"
"                                                                                           \n"
"       //                                                                                  \n"
"       // Angle between sun direction and view direction                                   \n"
"       //                                                                                  \n"
"                                                                                           \n"
"       float3 sunDirection = normalize( mul( MV, float4(SunDir, 0.0) ).xyz );              \n"
"       float  theta        = dot( sunDirection, viewDirection );                           \n"
"                                                                                           \n"
"       //                                                                                  \n"
"       // Phase1 and Phase2                                                                \n"
"       //                                                                                  \n"
"                                                                                           \n"
"       float phase1 = 1.0 + theta * theta;                                                 \n"
"       float phase2 = pow( rsqrt( HGg.y - HGg.z * theta ), 3 ) * HGg.x;                    \n"
"                                                                                           \n"
"       //                                                                                  \n"
"       // Extinction term                                                                  \n"
"       //                                                                                  \n"
"                                                                                           \n"
"       float3 extinction      = exp( -BetaRPlusBetaM * distance );                         \n"
"       float3 totalExtinction = extinction * Multipliers.yzw;                              \n"
"                                                                                           \n"
"       //                                                                                  \n"
"       // Inscattering term                                                                \n"
"       //                                                                                  \n"
"                                                                                           \n"
"       float3 betaRay = BetaDashR * phase1;                                                \n"
"       float3 betaMie = BetaDashM * phase2;                                                \n"
"                                                                                           \n"
"       float3 inscatter = (betaRay + betaMie) * OneOverBetaRPlusBetaM * (1.0 - extinction);\n"
"                                                                                           \n"
"       //                                                                                  \n"
"       // Apply inscattering contribution factors                                          \n"
"       //                                                                                  \n"
"                                                                                           \n"
"       inscatter *= Multipliers.x;                                                         \n"
"                                                                                           \n"
"       //                                                                                  \n"
"       // Scale with sun color & intensity                                                 \n"
"       //                                                                                  \n"
"                                                                                           \n"
"       inscatter       *= SunColorAndIntensity.xyz * SunColorAndIntensity.w;               \n"
"       totalExtinction *= SunColorAndIntensity.xyz * SunColorAndIntensity.w;               \n"
"                                                                                           \n"
"       //                                                                                  \n"
"       // Transform vertex and set its colors                                              \n"
"       //                                                                                  \n"
"                                                                                           \n"
"       vert2frag OUT;                                                                      \n"
"                                                                                           \n"
"       OUT.Position     = mul( MVP, IN.Position );                                         \n"
"       OUT.TerrainCoord = IN.TexCoord.xy;                                                  \n"
"       OUT.NormalCoord  = IN.TexCoord.xy;                                                  \n"
"       OUT.Lin          = inscatter;                                                       \n"
"       OUT.Fex          = totalExtinction;                                                 \n"
"                                                                                           \n"
"       return OUT;                                                                         \n"
"   }                                                                                       \n"

;

//-----------------------------------------------------------------------------
//  s_skyFragmentShaderSource
//-----------------------------------------------------------------------------
char HoffmanScatter::s_skyFragmentShaderSource[] =

"   struct vert2frag                                    \n"
"   {                                                   \n"
"       float4 Position     : POSITION;                 \n"
"       float2 TerrainCoord : TEXCOORD0;                \n"
"       float2 NormalCoord  : TEXCOORD1;                \n"
"       float3 Lin          : COLOR0;                   \n"
"       float3 Fex          : COLOR1;                   \n"
"   };                                                  \n"
"                                                       \n"
"   float3 HoffmanSkyShader( vert2frag IN ) : COLOR     \n"
"   {                                                   \n"
"       return IN.Lin;                                  \n"
"   }                                                   \n"

;

//-----------------------------------------------------------------------------
//  s_terrainFragmentShaderSource
//-----------------------------------------------------------------------------
char HoffmanScatter::s_terrainFragmentShaderSource[] =

"   struct vert2frag                                                            \n"
"   {                                                                           \n"
"       float4 Position     : POSITION;                                         \n"
"       float2 TerrainCoord : TEXCOORD0;                                        \n"
"       float2 NormalCoord  : TEXCOORD1;                                        \n"
"       float3 Lin          : COLOR0;                                           \n"
"       float3 Fex          : COLOR1;                                           \n"
"   };                                                                          \n"
"                                                                               \n"
"   float3 HoffmanTerrainShader ( vert2frag         IN                          \n"
"                               , uniform float3    LightVec                    \n"
"                               , uniform sampler2D TerrainMap                  \n"
"                               , uniform sampler2D NormalMap                   \n"
"                               ) : COLOR                                       \n"
"   {                                                                           \n"
"       float3 normal = 2.0 * (tex2D( NormalMap, IN.NormalCoord ).xyz - 0.5);   \n"
"       float3 light  = saturate( dot( normal, LightVec ) );                    \n"
"       float3 albedo = tex2D( TerrainMap, IN.TerrainCoord ).xyz;               \n"
"                                                                               \n"
"       return albedo * IN.Fex + IN.Lin;                                        \n"
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
HoffmanScatter::HoffmanScatter()
: m_HGg( 0.8f )
, m_inscatteringMultiplier( 1.0f )
, m_betaRayMultiplier( 1.0f )
, m_betaMieMultiplier( 1.0f )
, m_sunIntensity( 1.0f )
, m_turbitity( 2.0f )
{
    const float n = 1.0003f;
    const float N = 2.545e25f;
    const float pn = 0.035f;

    float fLambda[3], fLambda2[3], fLambda4[3];

    fLambda[0]  = 1.0f / 650e-9f;   // red
    fLambda[1]  = 1.0f / 570e-9f;   // green
    fLambda[2]  = 1.0f / 475e-9f;   // blue

    for( int i = 0; i < 3; ++i )
    {
        fLambda2[i] = fLambda[i]*fLambda[i];
        fLambda4[i] = fLambda2[i]*fLambda2[i];
    }

    CVector3 vLambda2( fLambda2[0], fLambda2[1], fLambda2[2] ); 
    CVector3 vLambda4( fLambda4[0], fLambda4[1], fLambda4[2] ); 

    // Rayleigh scattering constants

    const float fTemp = g_fPI * g_fPI * (n * n - 1.0f) * (n * n - 1.0f) * (6.0f + 3.0f * pn) / (6.0f - 7.0f * pn) / N;
    const float fBeta = 8.0f * fTemp * g_fPI / 3.0f;
 
    m_betaRay = fBeta * vLambda4;

    const float fBetaDash = fTemp / 2.0f;

    m_betaDashRay = fBetaDash * vLambda4;

    // Mie scattering constants

    const float T       = 2.0f;
    const float c       = (6.544f * T - 6.51f) * 1e-17f;
    const float fTemp2  = 0.434f * c * (2.0f * g_fPI) * (2.0f* g_fPI) * 0.5f;

    m_betaDashMie = fTemp2 * vLambda2;

    const float K[3]    = {0.685f, 0.679f, 0.670f};
    const float fTemp3  = 0.434f * c * g_fPI * (2.0f * g_fPI) * (2.0f * g_fPI);

    CVector3 vBetaMieTemp( K[0] * fLambda2[0], K[1] * fLambda2[1], K[2] * fLambda2[2] );

    m_betaMie = fTemp3 * vBetaMieTemp;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
HoffmanScatter::~HoffmanScatter()
{}

//-----------------------------------------------------------------------------
//  create
//-----------------------------------------------------------------------------
void HoffmanScatter::create()
{
    // Create sky and terrain shader programs

    m_cgVertexShader            = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_vertexShaderSource, SharedCgContext::getInstance().getVertexProfile(), "HoffmanShader", NULL );
    m_cgSkyFragmentShader       = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_skyFragmentShaderSource, SharedCgContext::getInstance().getFragmentProfile(), "HoffmanSkyShader", NULL );
    m_cgTerrainFragmentShader   = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_terrainFragmentShaderSource, SharedCgContext::getInstance().getFragmentProfile(), "HoffmanTerrainShader", NULL );

    m_validSkyLight = true;
    m_validAerialPerspective = true;

    cgGLLoadProgram( m_cgVertexShader );          if( CG_NO_ERROR != cgGetError() ) m_validSkyLight = m_validAerialPerspective = false;
    cgGLLoadProgram( m_cgSkyFragmentShader );     if( CG_NO_ERROR != cgGetError() ) m_validSkyLight = false;
    cgGLLoadProgram( m_cgTerrainFragmentShader ); if( CG_NO_ERROR != cgGetError() ) m_validAerialPerspective = false;

    m_cgPosition                = cgGetNamedParameter( m_cgVertexShader, "IN.Position" );
    m_cgTexCoord                = cgGetNamedParameter( m_cgVertexShader, "IN.TexCoord" );
    m_cgEyePos                  = cgGetNamedParameter( m_cgVertexShader, "EyePos" );
    m_cgSunDir                  = cgGetNamedParameter( m_cgVertexShader, "SunDir" );
    m_cgBetaRPlusBetaM          = cgGetNamedParameter( m_cgVertexShader, "BetaRPlusBetaM" );
    m_cgHGg                     = cgGetNamedParameter( m_cgVertexShader, "HGg" );
    m_cgBetaDashR               = cgGetNamedParameter( m_cgVertexShader, "BetaDashR" );
    m_cgBetaDashM               = cgGetNamedParameter( m_cgVertexShader, "BetaDashM" );
    m_cgOneOverBetaRPlusBetaM   = cgGetNamedParameter( m_cgVertexShader, "OneOverBetaRPlusBetaM" );
    m_cgMultipliers             = cgGetNamedParameter( m_cgVertexShader, "Multipliers" );
    m_cgSunColorAndIntensity    = cgGetNamedParameter( m_cgVertexShader, "SunColorAndIntensity" );
    m_cgMVP                     = cgGetNamedParameter( m_cgVertexShader, "MVP" );
    m_cgMV                      = cgGetNamedParameter( m_cgVertexShader, "MV" );

    m_cgLightVector = cgGetNamedParameter( m_cgTerrainFragmentShader, "LightVec" );
    m_cgTerrainMap  = cgGetNamedParameter( m_cgTerrainFragmentShader, "TerrainMap" );
    m_cgNormalMap   = cgGetNamedParameter( m_cgTerrainFragmentShader, "NormalMap" );

    // Log success

    if( true == m_validSkyLight )
        Logger::getInstance().logInfo( "HoffmanScatter", "create", "Skylight shader created successfully." );
    else
        Logger::getInstance().logError( "HoffmanScatter", "create", "Skylight shader creation failed." );

    if( true == m_validAerialPerspective )
        Logger::getInstance().logInfo( "HoffmanScatter", "create", "Aerial perspective shader created successfully." );
    else
        Logger::getInstance().logError( "HoffmanScatter", "create", "Aerial perspective shader creation failed." );
}

//-----------------------------------------------------------------------------
//  setSunIntensity
//-----------------------------------------------------------------------------
void HoffmanScatter::setSunIntensity( const float a_value )
{
    m_sunIntensity = a_value;
}

//-----------------------------------------------------------------------------
//  setMie
//-----------------------------------------------------------------------------
void HoffmanScatter::setMie( const float a_value )
{
    m_betaMieMultiplier = a_value;
}

//-----------------------------------------------------------------------------
//  setRayleigh
//-----------------------------------------------------------------------------
void HoffmanScatter::setRayleigh( const float a_value )
{
    m_betaRayMultiplier = a_value;
}

//-----------------------------------------------------------------------------
//  setHg
//-----------------------------------------------------------------------------
void HoffmanScatter::setHg( const float a_value )
{
    m_HGg = a_value;
}

//-----------------------------------------------------------------------------
//  setInscattering
//-----------------------------------------------------------------------------
void HoffmanScatter::setInscattering( const float a_value )
{
    m_inscatteringMultiplier = a_value;
}

//-----------------------------------------------------------------------------
//  setTurbidity
//-----------------------------------------------------------------------------
void HoffmanScatter::setTurbidity( const float a_value )
{
    m_turbitity = a_value;
}

//-----------------------------------------------------------------------------
//  isSkyLightValid
//-----------------------------------------------------------------------------
const bool HoffmanScatter::isSkyLightValid() const
{
    return m_validSkyLight;
}

//-----------------------------------------------------------------------------
//  isAerialPerspectiveValid
//-----------------------------------------------------------------------------
const bool HoffmanScatter::isAerialPerspectiveValid() const
{
    return m_validAerialPerspective;
}

//-----------------------------------------------------------------------------
//  renderSkyLight
//-----------------------------------------------------------------------------
void HoffmanScatter::renderSkyLight( const Camera& a_camera
                                   , const CVector3& a_sunDirection
                                   , const float* a_pVertices
                                   , const unsigned int* a_pIndices
                                   , const unsigned int a_indexCount
                                   )
{
    // Compute thetaS dependencies

    CVector3 vecZenith( 0.0f, 0.0f, 1.0f );

    float   fThetaS = CVector3::DotProduct( a_sunDirection, vecZenith );
            fThetaS = acosf( fThetaS );

    computeAttenuation( fThetaS );

    // Set shader constants

    setShaderConstants( a_camera, a_sunDirection );

    cgGLEnableClientState   ( m_cgPosition );
    cgGLSetParameterPointer ( m_cgPosition, 3, GL_FLOAT, 0, (void*)a_pVertices );

    // Enable programs

    cgGLEnableProfile       ( SharedCgContext::getInstance().getVertexProfile() );
    cgGLEnableProfile       ( SharedCgContext::getInstance().getFragmentProfile() );

    cgGLBindProgram         ( m_cgVertexShader );
    cgGLBindProgram         ( m_cgSkyFragmentShader );

    // Draw sky dome
        
    glDrawElements          ( GL_TRIANGLE_STRIP, a_indexCount, GL_UNSIGNED_INT, a_pIndices );

    // Disable programs

    cgGLDisableProfile      ( SharedCgContext::getInstance().getVertexProfile() );
    cgGLDisableProfile      ( SharedCgContext::getInstance().getFragmentProfile() );

    cgGLDisableClientState  ( m_cgPosition );
}

//-----------------------------------------------------------------------------
//  renderAerialPerspective
//-----------------------------------------------------------------------------
void HoffmanScatter::renderAerialPerspective( const Camera& a_camera
                                            , const CVector3& a_sunDirection
                                            , const float* a_pVertices
                                            , const float* a_pTexCoords
                                            , const unsigned int* a_pIndices
                                            , const unsigned int a_indexCount
                                            , const unsigned int a_terrainMap
                                            , const unsigned int a_normalMap
                                            )
{
    // Compute thetaS dependencies

    CVector3 vecZenith( 0.0f, 0.0f, 1.0f );

    float   fThetaS = CVector3::DotProduct( a_sunDirection, vecZenith );
            fThetaS = acosf( fThetaS );

    computeAttenuation( fThetaS );

    // Set shader constants

    setShaderConstants( a_camera, a_sunDirection );

    cgGLEnableClientState       ( m_cgPosition );
    cgGLEnableClientState       ( m_cgTexCoord );

    cgGLSetParameterPointer     ( m_cgPosition, 3, GL_FLOAT, 0, (void*)a_pVertices );
    cgGLSetParameterPointer     ( m_cgTexCoord, 3, GL_FLOAT, 0, (void*)a_pTexCoords );

    cgGLSetTextureParameter     ( m_cgTerrainMap, a_terrainMap );
    cgGLSetTextureParameter     ( m_cgNormalMap, a_normalMap );

    cgGLEnableTextureParameter  ( m_cgTerrainMap );
    cgGLEnableTextureParameter  ( m_cgNormalMap );

    cgGLSetParameter3fv         ( m_cgLightVector, a_sunDirection.GetCoordinates() );

    // Enable programs

    cgGLEnableProfile           ( SharedCgContext::getInstance().getVertexProfile() );
    cgGLEnableProfile           ( SharedCgContext::getInstance().getFragmentProfile() );

    cgGLBindProgram             ( m_cgVertexShader );
    cgGLBindProgram             ( m_cgTerrainFragmentShader );

    // Draw terrain strip

    glDrawElements              ( GL_TRIANGLE_STRIP, a_indexCount, GL_UNSIGNED_INT, a_pIndices );

    // Disable programs

    cgGLDisableProfile          ( SharedCgContext::getInstance().getVertexProfile() );
    cgGLDisableProfile          ( SharedCgContext::getInstance().getFragmentProfile() );

    cgGLDisableTextureParameter ( m_cgTerrainMap );
    cgGLDisableTextureParameter ( m_cgNormalMap );

    cgGLDisableClientState      ( m_cgTexCoord );
    cgGLDisableClientState      ( m_cgPosition );
}

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Private methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  computeAttenuation
//-----------------------------------------------------------------------------
void HoffmanScatter::computeAttenuation( const float a_theta )
{
    float fBeta = 0.04608365822050f * m_turbitity - 0.04586025928522f;
    float fTauR, fTauA, fTau[3];
    float m = 1.0f / (cosf( a_theta ) + 0.15f * powf( 93.885f - a_theta / g_fPI * 180.0f, -1.253f ));  // Relative Optical Mass
    float fLambda[3] = {0.65f, 0.57f, 0.475f }; 

    for( int i = 0; i < 3; ++i )
    {
        // Rayleigh Scattering
        // lambda in um.

        fTauR = expf( -m * 0.008735f * powf( fLambda[i], float(-4.08f) ) );

        // Aerosal (water + dust) attenuation
        // beta - amount of aerosols present 
        // alpha - ratio of small to large particle sizes. (0:4,usually 1.3)

        const float fAlpha = 1.3f;
        fTauA = expf(-m * fBeta * powf(fLambda[i], -fAlpha));  // lambda should be in um


        fTau[i] = fTauR * fTauA; 

    }

    m_sunColorAndIntensity = CVector4( fTau[0], fTau[1], fTau[2], m_sunIntensity * 100.0f );
}

//-----------------------------------------------------------------------------
//  setShaderConstants
//-----------------------------------------------------------------------------
void HoffmanScatter::setShaderConstants( const Camera& a_camera
                                       , const CVector3& a_sunDirection
                                       )
{
    float fReflectance = 0.1f;

    CVector3 vecBetaR           ( m_betaRay * m_betaRayMultiplier );
    CVector3 vecBetaDashR       ( m_betaDashRay * m_betaRayMultiplier );
    CVector3 vecBetaM           ( m_betaMie * m_betaMieMultiplier );
    CVector3 vecBetaDashM       ( m_betaDashMie * m_betaMieMultiplier );
    CVector3 vecBetaRM          ( vecBetaR + vecBetaM );
    CVector3 vecOneOverBetaRM   ( 1.0f / vecBetaRM.GetX(), 1.0f / vecBetaRM.GetY(), 1.0f / vecBetaRM.GetZ() );
    CVector3 vecG               ( 1.0f - m_HGg * m_HGg, 1.0f + m_HGg * m_HGg, 2.0f * m_HGg );
    CVector4 vecTermMulitpliers ( m_inscatteringMultiplier, 0.138f * fReflectance, 0.113f * fReflectance, 0.08f * fReflectance );

    cgGLSetParameter3fv         ( m_cgEyePos, a_camera.getPosition().GetCoordinates() );
    cgGLSetParameter3fv         ( m_cgSunDir, a_sunDirection.GetCoordinates() );
    cgGLSetParameter3fv         ( m_cgBetaRPlusBetaM, vecBetaRM.GetCoordinates() );
    cgGLSetParameter3fv         ( m_cgHGg, vecG.AccessCoordinates() );
    cgGLSetParameter3fv         ( m_cgBetaDashR, vecBetaDashR.GetCoordinates() );
    cgGLSetParameter3fv         ( m_cgBetaDashM, vecBetaDashM.GetCoordinates() );
    cgGLSetParameter3fv         ( m_cgOneOverBetaRPlusBetaM, vecOneOverBetaRM.GetCoordinates() );
    cgGLSetParameter4fv         ( m_cgMultipliers, vecTermMulitpliers.GetCoordinates() );
    cgGLSetParameter4fv         ( m_cgSunColorAndIntensity, m_sunColorAndIntensity.AccessCoordinates() );

    cgGLSetStateMatrixParameter ( m_cgMVP, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY );
    cgGLSetStateMatrixParameter ( m_cgMV, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY );
}
