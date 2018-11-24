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

#include        "PreethamScatter.hpp"
#include        "../logging/Logger.hpp"
#include        "../render/SharedCgContext.hpp"
#include        "../render/Camera.hpp"
#include        "../math/CMatrix4.h"
#include        <Cg/cgGL.h>

using Logging::Logger;

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Local methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  angleBetween
//-----------------------------------------------------------------------------
static double angleBetween( const double& a_thetav, const double& a_phiv, const double& a_theta, const double& a_phi )
{
    double cospsi = sin( a_thetav ) * sin( a_theta ) * cos( a_phi - a_phiv ) + cos( a_thetav ) * cos( a_theta );

    if( cospsi > 1.0 )
        return 0.0;

    if( cospsi < -1.0 )
        return g_dPI;

    return acos( cospsi );
}

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Static initialization
//*********************************************************************************************************************
//*********************************************************************************************************************

const double PreethamScatter::s_xDC[5][2] =
{
    { -0.0193, -0.2592 },
    { -0.0665,  0.0008 },
    { -0.0004,  0.2125 },
    { -0.0641, -0.8989 },
    { -0.0033,  0.0452 }
};

const double PreethamScatter::s_yDC[5][2] =
{
    { -0.0167, -0.2608 },
    { -0.0950,  0.0092 },
    { -0.0079,  0.2102 },
    { -0.0441, -1.6537 },
    { -0.0109,  0.0529 }
};

const double PreethamScatter::s_YDC[5][2] =
{
    {  0.1787, -1.4630 },
    { -0.3554,  0.4275 },
    { -0.0227,  5.3251 },
    {  0.1206, -2.5771 },
    { -0.0670,  0.3703 }
};

const double PreethamScatter::s_xZC[3][4] =
{
    {  0.00166, -0.00375,  0.00209, 0.00000 },
    { -0.02903,  0.06377, -0.03203, 0.00394 },
    {  0.11693, -0.21196,  0.06052, 0.25886 }
};  
  
const double PreethamScatter::s_yZC[3][4] =
{
    {  0.00275, -0.00610,  0.00317, 0.00000 },
    { -0.04214,  0.08970, -0.04153, 0.00516 },
    {  0.15346, -0.26756,  0.06670, 0.26688 }
};  
  
const double PreethamScatter::s_CM[9] =
{
     3.240479, -1.537150, -0.498535,
    -0.969256,  1.875992,  0.041556,
     0.055648, -0.204043,  1.057311
};

const double PreethamScatter::s_CIE_300_800[107][3] =
{
    #include "ciexyz31_300_800.table"
};

const double PreethamScatter::s_CIE_350_800[97][3] =
{
    #include "ciexyz31_350_800.table"
};

const double PreethamScatter::s_neta[] =
{
    #include "neta.table"
};

const double PreethamScatter::s_K[] =
{
    #include "K.table"
};

const double PreethamScatter::s_N21[] =
{
    #include "N21.table"
};

const double PreethamScatter::s_Ko[] =
{
    #include "Ko.table"
};

const double PreethamScatter::s_Kg[] =
{
    #include "Kg.table"
};

const double PreethamScatter::s_Kwa[] =
{
    #include "Kwa.table"
};

const double PreethamScatter::s_sol[] =
{
    #include "Sol.table"
};

const double PreethamScatter::s_V       = 4.0;
const double PreethamScatter::s_anisCor = 1.06;
const double PreethamScatter::s_N       = 2.545e25;
const double PreethamScatter::s_alpha1  = 0.83331810215394e-3;
const double PreethamScatter::s_alpha2  = 0.11360016092149e-3;

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Shader sources
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  s_skyVertexShaderSource
//-----------------------------------------------------------------------------
char PreethamScatter::s_skyVertexShaderSource[] =

"   struct app2vert                                                                                             \n"
"   {                                                                                                           \n"
"       float4 Position : POSITION;                                                                             \n"
"   };                                                                                                          \n"
"                                                                                                               \n"
"   struct vert2frag                                                                                            \n"
"   {                                                                                                           \n"
"       float4 Position : POSITION;                                                                             \n"
"       float3 Color    : COLOR0;                                                                               \n"
"   };                                                                                                          \n"
"                                                                                                               \n"
"   vert2frag PreethamSkyShader ( app2vert IN                                                                   \n"
"                               , uniform float3    a_sunDirection                                              \n"
"                               , uniform float2    a_thetaSun                                                  \n"
"                               , uniform float3    a_zenithDirection                                           \n"
"                               , uniform float3    a_zenithColor                                               \n"
"                               , uniform float3    a_A                                                         \n"
"                               , uniform float3    a_B                                                         \n"
"                               , uniform float3    a_C                                                         \n"
"                               , uniform float3    a_D                                                         \n"
"                               , uniform float3    a_E                                                         \n"
"                               , uniform float3x3  a_colorConvMat                                              \n"
"                               , uniform float4x4  a_worldViewProj                                             \n"
"                               , uniform float4x4  a_world                                                     \n"
"                               , uniform float     a_exposure                                                  \n"
"                               )                                                                               \n"
"   {                                                                                                           \n"
"       float3 direction = normalize( mul( a_world, IN.Position ).xyz );                                        \n"
"                                                                                                               \n"
"       float theta     = dot( a_zenithDirection, direction );                                                  \n"
"       float gamma     = dot( a_sunDirection, direction );                                                     \n"
"       float cos2gamma = gamma * gamma;                                                                        \n"
"             gamma     = acos( gamma );                                                                        \n"
"                                                                                                               \n"
"       //                                                                                                      \n"
"       // Compute sky chromaticity values and sky luminance                                                    \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
"       float3 num = (1.0 + a_A * exp( a_B / theta )) * (1.0 + a_C * exp( a_D * gamma ) + a_E * cos2gamma);     \n"
"       float3 den = (1.0 + a_A * exp( a_B )) * (1.0 + a_C * exp( a_D * a_thetaSun.x ) + a_E * a_thetaSun.y);   \n"
"       float3 xyY = num / den * a_zenithColor;                                                                 \n"
"                                                                                                               \n"
"       //                                                                                                      \n"
"       // Exposure function for luminance                                                                      \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
"       xyY.z = 1.0 - exp( -a_exposure * xyY.z );                                                               \n"
"                                                                                                               \n"
"       //                                                                                                      \n"
"       // Convert CIE xyY to CIE XYZ                                                                           \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
"       float3 XYZ;                                                                                             \n"
"                                                                                                               \n"
"       XYZ.x = (xyY.x / xyY.y) * xyY.z;                                                                        \n"
"       XYZ.y = xyY.z;                                                                                          \n"
"       XYZ.z = ((1.0 - xyY.x - xyY.y) / xyY.y) * xyY.z;                                                        \n"
"                                                                                                               \n"
"       //                                                                                                      \n"
"       // Transform vertex                                                                                     \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
"       vert2frag OUT;                                                                                          \n"
"                                                                                                               \n"
"       OUT.Position = mul( a_worldViewProj, IN.Position );                                                     \n"
"                                                                                                               \n"
"       //                                                                                                      \n"
"       // Convert CIE XYZ to RGB assuming D65 white point                                                      \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
"       OUT.Color = mul( a_colorConvMat, XYZ );                                                                 \n"
"                                                                                                               \n"
"       return OUT;                                                                                             \n"
"   }                                                                                                           \n"

;

//-----------------------------------------------------------------------------
//  s_skyFragmentShaderSource
//-----------------------------------------------------------------------------
char PreethamScatter::s_skyFragmentShaderSource[] =

"   struct vert2frag                                  \n"
"   {                                                 \n"
"       float4 Position : POSITION;                   \n"
"       float3 Color    : COLOR0;                     \n"
"   };                                                \n"
"                                                     \n"
"   float3 PreethamSkyShader( vert2frag IN ) : COLOR  \n"
"   {                                                 \n"
"       return IN.Color;                              \n"
"   }                                                 \n"

;

//-----------------------------------------------------------------------------
//  s_terrainVertexShaderSource
//-----------------------------------------------------------------------------
char PreethamScatter::s_terrainVertexShaderSource[] =

"   struct app2vert                                                                                             \n"
"   {                                                                                                           \n"
"       float4 Position : POSITION;                                                                             \n"
"       float3 TexCoord : TEXCOORD0;                                                                            \n"
"   };                                                                                                          \n"
"                                                                                                               \n"
"   struct vert2frag                                                                                            \n"
"   {                                                                                                           \n"
"       float4 Position     : POSITION;                                                                         \n"
"       float2 TerrainCoord : TEXCOORD0;                                                                        \n"
"       float2 NormalCoord  : TEXCOORD1;                                                                        \n"
"       float3 Inscatter    : COLOR0;                                                                           \n"
"       float3 Extinction   : COLOR1;                                                                           \n"
"   };                                                                                                          \n"
"                                                                                                               \n"
"   float evalFunc( float a_B, float a_s )                                                                      \n"
"   {                                                                                                           \n"
"       float result;                                                                                           \n"
"                                                                                                               \n"
"       if( abs( a_B * a_s ) < 0.01 )                                                                           \n"
"           result = a_s;                                                                                       \n"
"       else                                                                                                    \n"
"           result = (1.0 - exp( -a_B * a_s )) / a_B;                                                           \n"
"                                                                                                               \n"
"       return result;                                                                                          \n"
"   }                                                                                                           \n"
"                                                                                                               \n"
"   vert2frag PreethamTerrainShader ( app2vert IN                                                               \n"
"                                   , uniform float3    a_eyePos                                                \n"
"                                   , uniform float3    a_betaMie                                               \n"
"                                   , uniform float3    a_betaRay                                               \n"
"                                   , uniform float3    a_C1plusC2                                              \n"
"                                   , uniform float2    a_alpha     // x = Alpha1, y = Alpha2                   \n"
"                                   , uniform float2    a_bins      // x = bins, y = bins+1                     \n"
"                                   , uniform float3    a_constants // x,y = constTerm1,2, z = inscatter scale  \n"
"                                   , uniform float3x3  a_colorConvMat                                          \n"
"                                   , uniform float4x4  a_worldViewProj                                         \n"
"                                   , uniform float4x4  a_worldView                                             \n"
"                                   , uniform float3    a_S0Mie[36]                                             \n"
"                                   , uniform float3    a_S0Ray[36]                                             \n"
"                                   )                                                                           \n"
"   {                                                                                                           \n"
"       //                                                                                                      \n"
"       // Constants                                                                                            \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
"       const float epsilon = 1e-4;                                                                             \n"
"                                                                                                               \n"
"       //                                                                                                      \n"
"       // Inscattering                                                                                         \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
"       float3 eyeVert   = mul( a_worldView, IN.Position ).xyz;                                                 \n"
"       float3 direction = normalize( eyeVert );                                                                \n"
"                                                                                                               \n"
"       float thetav = ((-direction.z + 1.0) / 2.0) * a_bins.x - epsilon;                                       \n"
"             thetav = max( thetav, 0.0 );                                                                      \n"
"                                                                                                               \n"
//"     float2 dirTemp   = float2( direction.x, direction.y );                                                  \n"
"       float2 dirTemp   = normalize( float2( direction.x, direction.y ) );                                     \n"
"                                                                                                               \n"
"       if( dirTemp.y < 0.0 )                                                                                   \n"
"           dirTemp.x = -(dirTemp.x + 2.0);                                                                     \n"
"                                                                                                               \n"
"       float phiv = ((-dirTemp.x + 1.0) / 4.0) * a_bins.x - epsilon;                                           \n"
"             phiv = max( phiv, 0.0 );                                                                          \n"
"                                                                                                               \n"
"       int i = thetav;                                                                                         \n"
"       float u = thetav - i;                                                                                   \n"
"                                                                                                               \n"
"       int j = phiv;                                                                                           \n"
"       float v = phiv - j;                                                                                     \n"
"                                                                                                               \n"
"       int4 indices;                                                                                           \n"
"       indices.x = i * a_bins.y + j;                                                                           \n"
"       indices.y = (i + 1) * a_bins.y + j;                                                                     \n"
"       indices.z = i * a_bins.y + j + 1;                                                                       \n"
"       indices.w = (i + 1) * a_bins.y + j + 1;                                                                 \n"
"                                                                                                               \n"
"       float4 factors;                                                                                         \n"
"       factors.x = (1.0 - u) * (1.0 - v);                                                                      \n"
"       factors.y = u * (1.0 - v);                                                                              \n"
"       factors.z = (1.0 - u) * v;                                                                              \n"
"       factors.w = u * v;                                                                                      \n"
"                                                                                                               \n"
"       float3 S0Mie = factors.x * a_S0Mie[ indices.x ]                                                         \n"
"                    + factors.y * a_S0Mie[ indices.y ]                                                         \n"
"                    + factors.z * a_S0Mie[ indices.z ]                                                         \n"
"                    + factors.w * a_S0Mie[ indices.w ];                                                        \n"
"       float3 S0Ray = factors.x * a_S0Ray[ indices.x ]                                                         \n"
"                    + factors.y * a_S0Ray[ indices.y ]                                                         \n"
"                    + factors.z * a_S0Ray[ indices.z ]                                                         \n"
"                    + factors.w * a_S0Ray[ indices.w ];                                                        \n"
"                                                                                                               \n"
"       float s = length( eyeVert );                                                                            \n"
"                                                                                                               \n"
"       float B1 = a_alpha.x * direction.z;                                                                     \n"
"       float B2 = a_alpha.y * direction.z;                                                                     \n"
"                                                                                                               \n"
"       float3 IMie = a_constants.x * ((1.0 - exp( -(B1 + a_C1plusC2) * s) ) / (B1 + a_C1plusC2));              \n"
"       float3 IRay = a_constants.y * ((1.0 - exp( -(B2 + a_C1plusC2) * s) ) / (B2 + a_C1plusC2));              \n"
"                                                                                                               \n"
"       float3 Lin = S0Mie * IMie + S0Ray * IRay;                                                               \n"
"                                                                                                               \n"
"       Lin *= a_constants.z;                                                                                   \n"
"                                                                                                               \n"
"       //                                                                                                      \n"
"       // Extinction                                                                                           \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
//"     if( 0.0 == direction.z )                                                                                \n"
//"         B1 = B2 = epsilon;                                                                                  \n"
"                                                                                                               \n"
"       float3 Fex = exp( -a_betaMie * a_constants.x * evalFunc( B1, s ) ) *                                    \n"
"                    exp( -a_betaRay * a_constants.y * evalFunc( B2, s ) );                                     \n"
"                                                                                                               \n"
"       //                                                                                                      \n"
"       // Transform vertex                                                                                     \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
"       vert2frag OUT;                                                                                          \n"
"                                                                                                               \n"
"       OUT.Position     = mul( a_worldViewProj, IN.Position );                                                 \n"
"       OUT.TerrainCoord = IN.TexCoord.xy;                                                                      \n"
"       OUT.NormalCoord  = IN.TexCoord.xy;                                                                      \n"
"                                                                                                               \n"
"       //                                                                                                      \n"
"       // Convert CIE XYZ to RGB assuming D65 white point                                                      \n"
"       //                                                                                                      \n"
"                                                                                                               \n"
"       OUT.Inscatter   = mul( a_colorConvMat, Lin );                                                           \n"
"       OUT.Extinction  = mul( a_colorConvMat, Fex );                                                           \n"
"                                                                                                               \n"
"       return OUT;                                                                                             \n"
"   }                                                                                                           \n"

;

//-----------------------------------------------------------------------------
//  s_terrainFragmentShaderSource
//-----------------------------------------------------------------------------
char PreethamScatter::s_terrainFragmentShaderSource[] =

"   struct vert2frag                                                            \n"
"   {                                                                           \n"
"       float4 Position     : POSITION;                                         \n"
"       float2 TerrainCoord : TEXCOORD0;                                        \n"
"       float2 NormalCoord  : TEXCOORD1;                                        \n"
"       float3 Inscatter    : COLOR0;                                           \n"
"       float3 Extinction   : COLOR1;                                           \n"
"   };                                                                          \n"
"                                                                               \n"
"   float3 PreethamTerrainShader( vert2frag         IN                          \n"
"                               , uniform float3    a_lightVec                  \n"
"                               , uniform sampler2D a_terrainMap                \n"
"                               , uniform sampler2D a_normalMap                 \n"
"                               ) : COLOR                                       \n"
"   {                                                                           \n"
"       float3 normal = 2.0 * (tex2D( a_normalMap, IN.NormalCoord ).xyz - 0.5); \n"
"       float3 light  = saturate( dot( normal, a_lightVec ) );                  \n"
"       float3 albedo = tex2D( a_terrainMap, IN.TerrainCoord ).xyz;             \n"
"                                                                               \n"
"       return light * albedo * IN.Extinction + IN.Inscatter;                   \n"
//"     return IN.Inscatter;                                                    \n"
//"     return IN.Extinction;                                                   \n"
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
PreethamScatter::PreethamScatter()
{
    m_thetaSun = -1.0;
    m_phiSun   = -1.0;
    m_sunSolidAngle = 0.25 * g_dPI * 1.39 * 1.39 / (150.0 * 150.0);

    // Init neta table

    for( int i = 0; i < 1801; ++i )
        m_netaTable[i].SetVector( static_cast< float >( s_neta[ i * 3 + 0 ] )
                                , static_cast< float >( s_neta[ i * 3 + 1 ] )
                                , static_cast< float >( s_neta[ i * 3 + 2 ] )
                                );

    // Init aerial perspective data 

    setTurbidity( 2.0 );
    setSun( 0.0, 0.0 );

    m_constants.SetVector( static_cast< float >( c_thetaBins ), static_cast< float >( c_phiBins ), 0.1f, 0.007f ); // thetaBins, phiBins, exposure, inscatter scale
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
PreethamScatter::~PreethamScatter()
{}

//-----------------------------------------------------------------------------
//  create
//-----------------------------------------------------------------------------
void PreethamScatter::create()
{
    // Create sky shader programs

    m_cgSkyVertexShader     = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_skyVertexShaderSource, SharedCgContext::getInstance().getVertexProfile(), "PreethamSkyShader", NULL );
    m_cgSkyFragmentShader   = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_skyFragmentShaderSource, SharedCgContext::getInstance().getFragmentProfile(), "PreethamSkyShader", NULL );

    m_validSkyLight = true;

    cgGLLoadProgram( m_cgSkyVertexShader );   if( CG_NO_ERROR != cgGetError() ) m_validSkyLight = false;
    cgGLLoadProgram( m_cgSkyFragmentShader ); if( CG_NO_ERROR != cgGetError() ) m_validSkyLight = false;

    m_cgSkyPosition     = cgGetNamedParameter( m_cgSkyVertexShader, "IN.Position" );
    m_cgSkySunDir       = cgGetNamedParameter( m_cgSkyVertexShader, "a_sunDirection" );
    m_cgSkyThetaS       = cgGetNamedParameter( m_cgSkyVertexShader, "a_thetaSun" );
    m_cgSkyZenith       = cgGetNamedParameter( m_cgSkyVertexShader, "a_zenithDirection" );
    m_cgSkyZenith_xyY   = cgGetNamedParameter( m_cgSkyVertexShader, "a_zenithColor" );
    m_cgSkyA_xyY        = cgGetNamedParameter( m_cgSkyVertexShader, "a_A" );
    m_cgSkyB_xyY        = cgGetNamedParameter( m_cgSkyVertexShader, "a_B" );
    m_cgSkyC_xyY        = cgGetNamedParameter( m_cgSkyVertexShader, "a_C" );
    m_cgSkyD_xyY        = cgGetNamedParameter( m_cgSkyVertexShader, "a_D" );
    m_cgSkyE_xyY        = cgGetNamedParameter( m_cgSkyVertexShader, "a_E" );
    m_cgSkyCM           = cgGetNamedParameter( m_cgSkyVertexShader, "a_colorConvMat" );
    m_cgSkyMVP          = cgGetNamedParameter( m_cgSkyVertexShader, "a_worldViewProj" );
    m_cgSkyM            = cgGetNamedParameter( m_cgSkyVertexShader, "a_world" );
    m_cgSkyExposure     = cgGetNamedParameter( m_cgSkyVertexShader, "a_exposure" );

    // Create terrain shader programs

    m_cgTerrainVertexShader     = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_terrainVertexShaderSource, SharedCgContext::getInstance().getVertexProfile(), "PreethamTerrainShader", NULL );
    m_cgTerrainFragmentShader   = cgCreateProgram( SharedCgContext::getInstance().getContext(), CG_SOURCE, s_terrainFragmentShaderSource, SharedCgContext::getInstance().getFragmentProfile(), "PreethamTerrainShader", NULL );

    m_validAerialPerspective = true;

    cgGLLoadProgram( m_cgTerrainVertexShader );   if( CG_NO_ERROR != cgGetError() ) m_validAerialPerspective = false;
    cgGLLoadProgram( m_cgTerrainFragmentShader ); if( CG_NO_ERROR != cgGetError() ) m_validAerialPerspective = false;

    m_cgTerrainPosition     = cgGetNamedParameter( m_cgTerrainVertexShader, "IN.Position" );
    m_cgTerrainTexCoord     = cgGetNamedParameter( m_cgTerrainVertexShader, "IN.TexCoord" );
    m_cgTerrainEyePos       = cgGetNamedParameter( m_cgTerrainVertexShader, "a_eyePos" );
    m_cgTerrainBetaMie      = cgGetNamedParameter( m_cgTerrainVertexShader, "a_betaMie" );
    m_cgTerrainBetaRay      = cgGetNamedParameter( m_cgTerrainVertexShader, "a_betaRay" );
    m_cgTerrainAlpha        = cgGetNamedParameter( m_cgTerrainVertexShader, "a_alpha" );
    m_cgTerrainBins         = cgGetNamedParameter( m_cgTerrainVertexShader, "a_bins" );
    m_cgTerrainConstants    = cgGetNamedParameter( m_cgTerrainVertexShader, "a_constants" );
    m_cgTerrainC1plusC2     = cgGetNamedParameter( m_cgTerrainVertexShader, "a_C1plusC2" );
    m_cgTerrainCM           = cgGetNamedParameter( m_cgTerrainVertexShader, "a_colorConvMat" );
    m_cgTerrainMVP          = cgGetNamedParameter( m_cgTerrainVertexShader, "a_worldViewProj" );
    m_cgTerrainMV           = cgGetNamedParameter( m_cgTerrainVertexShader, "a_worldView" );
    m_cgTerrainS0_Mie       = cgGetNamedParameter( m_cgTerrainVertexShader, "a_S0Mie" );
    m_cgTerrainS0_Ray       = cgGetNamedParameter( m_cgTerrainVertexShader, "a_S0Ray" );

    m_cgTerrainLightVector  = cgGetNamedParameter( m_cgTerrainFragmentShader, "a_lightVec" );
    m_cgTerrainMap          = cgGetNamedParameter( m_cgTerrainFragmentShader, "a_terrainMap" );
    m_cgTerrainNormalMap    = cgGetNamedParameter( m_cgTerrainFragmentShader, "a_normalMap" );

    // Log success

    if( true == m_validSkyLight )
        Logger::getInstance().logInfo( "PreethamScatter", "create", "Skylight shader created successfully." );
    else
        Logger::getInstance().logError( "PreethamScatter", "create", "Skylight shader creation failed." );

    if( true == m_validAerialPerspective )
        Logger::getInstance().logInfo( "PreethamScatter", "create", "Aerial perspective shader created successfully." );
    else
        Logger::getInstance().logError( "PreethamScatter", "create", "Aerial perspective shader creation failed." );
}

//-----------------------------------------------------------------------------
//  setTurbidity
//-----------------------------------------------------------------------------
void PreethamScatter::setTurbidity( const double a_turbidity )
{
    // Calculate Perez function coefficients

    m_turbitity = a_turbidity;

    m_Ax = s_xDC[0][0] * m_turbitity + s_xDC[0][1];
    m_Bx = s_xDC[1][0] * m_turbitity + s_xDC[1][1];
    m_Cx = s_xDC[2][0] * m_turbitity + s_xDC[2][1];
    m_Dx = s_xDC[3][0] * m_turbitity + s_xDC[3][1];
    m_Ex = s_xDC[4][0] * m_turbitity + s_xDC[4][1];

    m_Ay = s_yDC[0][0] * m_turbitity + s_yDC[0][1];
    m_By = s_yDC[1][0] * m_turbitity + s_yDC[1][1];
    m_Cy = s_yDC[2][0] * m_turbitity + s_yDC[2][1];
    m_Dy = s_yDC[3][0] * m_turbitity + s_yDC[3][1];
    m_Ey = s_yDC[4][0] * m_turbitity + s_yDC[4][1];

    m_AY = s_YDC[0][0] * m_turbitity + s_YDC[0][1];
    m_BY = s_YDC[1][0] * m_turbitity + s_YDC[1][1];
    m_CY = s_YDC[2][0] * m_turbitity + s_YDC[2][1];
    m_DY = s_YDC[3][0] * m_turbitity + s_YDC[3][1];
    m_EY = s_YDC[4][0] * m_turbitity + s_YDC[4][1];

    // Calculate Mie and Rayleigh scattering coefficients

    double b_m[101], b_p[101];
    double b_m_ang_prefix[101];
    double b_p_ang_prefix[101];

    double c = (0.06544204545455 * m_turbitity - 0.06509886363636) * 1e-15;

    m_betaM     .SetVector( 0.0 );
    m_betaP     .SetVector( 0.0 );
    m_betaMAng  .SetVector( 0.0 );
    m_betaPAng  .SetVector( 0.0 );

    double lambda;
    int i;

    for( lambda = 300.0, i = 0; lambda <= 800.0; lambda += 5.0, ++i )
    {
        double lambdasi = lambda * 1e-9;  // Converstion to SI units
        double Nlambda4 = s_N * lambdasi * lambdasi * lambdasi * lambdasi;

        // Rayleigh total scattering coefficient

        double n2_1 = s_N21[i]; 

        b_m[i] = 8.0 * g_dPI * g_dPI * g_dPI * n2_1 * s_anisCor / (3.0 * Nlambda4);

        // Mie total scattering coefficient 

        double K = s_K[i];

        b_p[i] = 0.434 * g_dPI * c * pow( 2.0 * g_dPI / lambdasi, s_V - 2.0 ) * K * pow( 0.01, s_V - 3.0 );

        // Rayleigh angular scattering coefficient 
        // Needs to be multiplied by (1+0.9324cos^2(theta)) to get exact angular scattering constant 

        b_m_ang_prefix[i] = 2.0 * g_dPI * g_dPI * n2_1 * s_anisCor * 0.7629 / (3.0 * Nlambda4);

        // Mie angular scattering coefficient 
        // Needs to be multiplied by neta(theta,lambda) to get exact angular scattering constant term 

        b_p_ang_prefix[i] = 0.217 * c * pow( 2.0 * g_dPI / lambdasi, s_V - 2.0 ) * pow( 0.01,s_V - 3.0 );

        // Convert spectral curves to CIE XYZ

        m_betaM[0] += static_cast< float >( b_m[i] * s_CIE_300_800[i][0] );
        m_betaM[1] += static_cast< float >( b_m[i] * s_CIE_300_800[i][1] );
        m_betaM[2] += static_cast< float >( b_m[i] * s_CIE_300_800[i][2] );

        m_betaP[0] += static_cast< float >( b_p[i] * s_CIE_300_800[i][0] );
        m_betaP[1] += static_cast< float >( b_p[i] * s_CIE_300_800[i][1] );
        m_betaP[2] += static_cast< float >( b_p[i] * s_CIE_300_800[i][2] );

        m_betaMAng[0] += static_cast< float >( b_m_ang_prefix[i] * s_CIE_300_800[i][0] );
        m_betaMAng[1] += static_cast< float >( b_m_ang_prefix[i] * s_CIE_300_800[i][1] );
        m_betaMAng[2] += static_cast< float >( b_m_ang_prefix[i] * s_CIE_300_800[i][2] );

        m_betaPAng[0] += static_cast< float >( b_p_ang_prefix[i] * s_CIE_300_800[i][0] );
        m_betaPAng[1] += static_cast< float >( b_p_ang_prefix[i] * s_CIE_300_800[i][1] );
        m_betaPAng[2] += static_cast< float >( b_p_ang_prefix[i] * s_CIE_300_800[i][2] );
    }

    computeAttenuatedSunlight();
}

//-----------------------------------------------------------------------------
//  setScaleFactors
//-----------------------------------------------------------------------------
void PreethamScatter::setScaleFactors( const float a_exposure, const float a_scaleInscatter )
{
    m_constants.SetVector( static_cast< float >( c_thetaBins ), static_cast< float >( c_phiBins ), a_exposure, a_scaleInscatter );
}

//-----------------------------------------------------------------------------
//  isSkyLightValid
//-----------------------------------------------------------------------------
const bool PreethamScatter::isSkyLightValid() const
{
    return m_validSkyLight;
}

//-----------------------------------------------------------------------------
//  isAerialPerspectiveValid
//-----------------------------------------------------------------------------
const bool PreethamScatter::isAerialPerspectiveValid() const
{
    return m_validAerialPerspective;
}

//-----------------------------------------------------------------------------
//  renderSkyLight
//-----------------------------------------------------------------------------
void PreethamScatter::renderSkyLight( const Camera& a_camera
                                    , const CVector3& a_sunDirection
                                    , const float* a_pVertices
                                    , const unsigned int* a_pIndices
                                    , const unsigned int a_indexCount
                                    )
{
    CVector3 vecZenith( 0.0f, 0.0f, 1.0f );
    CMatrix4 matIdentitiy;

    // Compute thetaS and phiS dependencies

    setSun( acos( a_sunDirection.GetZ() ), atan2( a_sunDirection.GetY(), a_sunDirection.GetX() ) );

    // Set shader constants

    cgGLEnableClientState       ( m_cgSkyPosition );
    cgGLSetParameterPointer     ( m_cgSkyPosition, 3, GL_FLOAT, 0, (void*)a_pVertices );

    cgGLSetParameter3fv         ( m_cgSkySunDir, a_sunDirection.GetCoordinates() );
    cgGLSetParameter2d          ( m_cgSkyThetaS, m_thetaSun, cos( m_thetaSun ) * cos( m_thetaSun ) );

    cgGLSetParameter3fv         ( m_cgSkyZenith, vecZenith.GetCoordinates() );
    cgGLSetParameter3d          ( m_cgSkyZenith_xyY, m_xZenith, m_yZenith, m_YZenith    );

    cgGLSetParameter3d          ( m_cgSkyA_xyY, m_Ax, m_Ay, m_AY );
    cgGLSetParameter3d          ( m_cgSkyB_xyY, m_Bx, m_By, m_BY );
    cgGLSetParameter3d          ( m_cgSkyC_xyY, m_Cx, m_Cy, m_CY );
    cgGLSetParameter3d          ( m_cgSkyD_xyY, m_Dx, m_Dy, m_DY );
    cgGLSetParameter3d          ( m_cgSkyE_xyY, m_Ex, m_Ey, m_EY );

    cgGLSetMatrixParameterdr    ( m_cgSkyCM, s_CM );

    cgGLSetStateMatrixParameter ( m_cgSkyMVP, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY );

    cgGLSetMatrixParameterfc    ( m_cgSkyM, (const float*)&matIdentitiy );

    cgGLSetParameter1f          ( m_cgSkyExposure, m_constants.GetZ() );

    // Enable programs

    cgGLEnableProfile           ( SharedCgContext::getInstance().getVertexProfile() );
    cgGLEnableProfile           ( SharedCgContext::getInstance().getFragmentProfile() );

    cgGLBindProgram             ( m_cgSkyVertexShader );
    cgGLBindProgram             ( m_cgSkyFragmentShader );

    // Draw dome

    glDrawElements              ( GL_TRIANGLE_STRIP, a_indexCount, GL_UNSIGNED_INT, a_pIndices );

    // Disable programs

    cgGLDisableProfile          ( SharedCgContext::getInstance().getVertexProfile() );
    cgGLDisableProfile          ( SharedCgContext::getInstance().getFragmentProfile() );

    cgGLDisableClientState      ( m_cgSkyPosition );
}

//-----------------------------------------------------------------------------
//  renderAerialPerspective
//-----------------------------------------------------------------------------
void PreethamScatter::renderAerialPerspective( const Camera& a_camera
                                             , const CVector3& a_sunDirection
                                             , const float* a_pVertices
                                             , const float* a_pTexCoords
                                             , const unsigned int* a_pIndices
                                             , const unsigned int a_indexCount
                                             , const unsigned int a_terrainMap
                                             , const unsigned int a_normalMap
                                             )
{
    // Compute thetaS and phiS dependencies

    setSun( acos( a_sunDirection.GetZ() ), atan2( a_sunDirection.GetY(), a_sunDirection.GetX() ) );

    // Set shader constants

    cgGLEnableClientState       ( m_cgTerrainPosition );
    cgGLEnableClientState       ( m_cgTerrainTexCoord );

    cgGLSetParameterPointer     ( m_cgTerrainPosition, 3, GL_FLOAT, 0, (void*)a_pVertices );
    cgGLSetParameterPointer     ( m_cgTerrainTexCoord, 3, GL_FLOAT, 0, (void*)a_pTexCoords );

    cgGLSetTextureParameter     ( m_cgTerrainMap, a_terrainMap );
    cgGLSetTextureParameter     ( m_cgTerrainNormalMap, a_normalMap );

    cgGLEnableTextureParameter  ( m_cgTerrainMap );
    cgGLEnableTextureParameter  ( m_cgTerrainNormalMap );

    cgGLSetParameter3fv         ( m_cgTerrainLightVector, a_sunDirection.GetCoordinates() );
    cgGLSetParameter3fv         ( m_cgTerrainEyePos, a_camera.getPosition().GetCoordinates() );
    cgGLSetParameter3fv         ( m_cgTerrainBetaMie, m_betaP.GetCoordinates() );
    cgGLSetParameter3fv         ( m_cgTerrainBetaRay, m_betaM.GetCoordinates() );

    float H1 = static_cast< float >( exp( -s_alpha1 * a_camera.getPosition().GetZ() ) );
    float H2 = static_cast< float >( exp( -s_alpha2 * a_camera.getPosition().GetZ() ) );

    cgGLSetParameter3f          ( m_cgTerrainC1plusC2
                                , m_betaP.GetX() * H1 + m_betaM.GetX() * H2 
                                , m_betaP.GetY() * H1 + m_betaM.GetY() * H2 
                                , m_betaP.GetZ() * H1 + m_betaM.GetZ() * H2 
                                );

    cgGLSetParameter2f          ( m_cgTerrainAlpha
                                , static_cast< float >( s_alpha1 )
                                , static_cast< float >( s_alpha2 )
                                );

    cgGLSetParameter2f          ( m_cgTerrainBins
                                , static_cast< float >( c_thetaBins )
                                , static_cast< float >( c_thetaBins + 1 )
                                );

    cgGLSetParameter3f          ( m_cgTerrainConstants
                                , H1
                                , H2
                                , m_constants.GetW()
                                );

    cgGLSetMatrixParameterdr    ( m_cgTerrainCM, s_CM );

    cgGLSetStateMatrixParameter ( m_cgTerrainMVP, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY );
    cgGLSetStateMatrixParameter ( m_cgTerrainMV, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY );

    cgGLSetParameterArray3f     ( m_cgTerrainS0_Mie, 0, 0, (float*)&m_S0_Mie );
    cgGLSetParameterArray3f     ( m_cgTerrainS0_Ray, 0, 0, (float*)&m_S0_Ray );

    // Enable programs

    cgGLEnableProfile           ( SharedCgContext::getInstance().getVertexProfile() );
    cgGLEnableProfile           ( SharedCgContext::getInstance().getFragmentProfile() );

    cgGLBindProgram             ( m_cgTerrainVertexShader );
    cgGLBindProgram             ( m_cgTerrainFragmentShader );

    // Draw terrain strip

    glDrawElements              ( GL_TRIANGLE_STRIP, a_indexCount, GL_UNSIGNED_INT, (void*)a_pIndices );

    // Disable programs

    cgGLDisableProfile          ( SharedCgContext::getInstance().getVertexProfile() );
    cgGLDisableProfile          ( SharedCgContext::getInstance().getFragmentProfile() );

    cgGLDisableTextureParameter ( m_cgTerrainMap );
    cgGLDisableTextureParameter ( m_cgTerrainNormalMap );

    cgGLDisableClientState      ( m_cgTerrainTexCoord );
    cgGLDisableClientState      ( m_cgTerrainPosition );
}

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Private methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  computeChromaticity
//-----------------------------------------------------------------------------
const double PreethamScatter::computeChromaticity( const double m[][4] ) const
{
    double T2 = m_turbitity * m_turbitity;
    double t2 = m_thetaSun * m_thetaSun;
    double t3 = t2 * m_thetaSun;

    return  (T2 * m[0][0] + m_turbitity * m[1][0] + m[2][0]) * t3 +
            (T2 * m[0][1] + m_turbitity * m[1][1] + m[2][1]) * t2 +
            (T2 * m[0][2] + m_turbitity * m[1][2] + m[2][2]) * m_thetaSun +
            (T2 * m[0][3] + m_turbitity * m[1][3] + m[2][3]);
}

//-----------------------------------------------------------------------------
//  perezFunction
//-----------------------------------------------------------------------------
const double PreethamScatter::perezFunction( const double a_A, const double a_B, const double a_C, const double a_D, const double a_E
                                           , const double a_theta, const double a_gamma
                                           ) const
{
    double e0 = a_B / cos( a_theta );
    double e1 = a_D * a_gamma;
    double e2 = cos( a_gamma );
           e2 *= e2;

    double f1 = (1 + a_A * exp( e0 )) * (1 + a_C * exp( e1 ) + a_E * e2); 

    e0 = a_B;
    e1 = a_D * m_thetaSun;
    e2 = cos( m_thetaSun );
    e2 *= e2;

    double f2 = (1 + a_A * exp( e0 )) * (1 + a_C * exp( e1 ) + a_E * e2); 

    return f1 / f2;
}

//-----------------------------------------------------------------------------
//  computeAttenuatedSunlight
//-----------------------------------------------------------------------------
void PreethamScatter::computeAttenuatedSunlight()
{
    double data[91];  // (800 - 350) / 5  + 1
    double beta = 0.04608365822050 * m_turbitity - 0.04586025928522;

    double m = 1.0 / (cos( m_thetaSun ) + 0.15 * pow( 93.885 - m_thetaSun * g_dRadToDeg, -1.253 ));  // Relative Optical Mass

    m_sunSpectralRad.SetVector( 0.0 );

    int i;
    double lambda;

    for( i = 0, lambda = 350; i < 91; ++i, lambda += 5.0 )
    {
        // Rayleigh Scattering

        double tauR = exp( -m * 0.008735 * pow( lambda / 1000.0, -4.08 ) );

        // Aerosal (water + dust) attenuation
        // beta - amount of aerosols present
        // alpha - ratio of small to large particle sizes. (0:4,usually 1.3)

        const double alpha = 1.3;

        double tauA = exp( -m * beta * pow( lambda / 1000.0, -alpha ) );  // lambda should be in um

        // Attenuation due to ozone absorption
        // lOzone - amount of ozone in cm(NTP)

        const double lOzone = 0.35;

        double tauO = exp( -m * s_Ko[i] * lOzone );

        // Attenuation due to mixed gases absorption

        double tauG = exp( -1.41 * s_Kg[i] * m / pow( 1.0 + 118.93 * s_Kg[i] * m, 0.45 ) );

        // Attenuation due to water vapor absorbtion
        // w - precipitable water vapor in centimeters (standard = 2)

        const double w = 2.0;

        double tauWA = exp( -0.2385 * s_Kwa[i] * w * m / pow( 1.0 + 20.07 * s_Kwa[i] * w * m, 0.45 ) );

        data[i] = 100.0 * s_sol[i] * tauR * tauA * tauO * tauG * tauWA;  // 100 comes from solCurve being in wrong units. 

        // Convert spectral curve to CIE XYZ

        m_sunSpectralRad[0] += static_cast< float >( data[i] * s_CIE_350_800[i][0] );
        m_sunSpectralRad[1] += static_cast< float >( data[i] * s_CIE_350_800[i][1] );
        m_sunSpectralRad[2] += static_cast< float >( data[i] * s_CIE_350_800[i][2] );
    }
}

//-----------------------------------------------------------------------------
//  getSkySpectralRadiance
//-----------------------------------------------------------------------------
void PreethamScatter::getSkySpectralRadiance( CVector3& a_color, const double a_theta, const double a_phi ) const
{
    double gamma = angleBetween( a_theta, a_phi, m_thetaSun, m_phiSun );

    double x = m_xZenith * perezFunction( m_Ax, m_Bx, m_Cx, m_Dx, m_Ex, a_theta, gamma );
    double y = m_yZenith * perezFunction( m_Ay, m_By, m_Cy, m_Dy, m_Ey, a_theta, gamma );
    double Y = m_YZenith * perezFunction( m_AY, m_BY, m_CY, m_DY, m_EY, a_theta, gamma );

//    Y = 1.0 - exp( -m_constants.GetZ() * Y );

    double X = (x / y) * Y;
    double Z = ((1.0 - x - y) / y) * Y;

    a_color.SetVector( static_cast< float >( X )
                     , static_cast< float >( Y )
                     , static_cast< float >( Z )
                     );
}

//-----------------------------------------------------------------------------
//  setSun
//-----------------------------------------------------------------------------
void PreethamScatter::setSun( const double a_theta, const double a_phi )
{
    // Check need for update

    bool bInitA0 = false;

    if( a_theta != m_thetaSun )
        bInitA0 = true;

    if( a_phi != m_phiSun )
        bInitA0 = true;

    m_thetaSun = a_theta;
    m_phiSun   = a_phi;

    // Calculate sun luminance and chromaticity values

    double dChi = (4.0 / 9.0 - m_turbitity / 120.0) * (g_dPI - 2.0 * m_thetaSun);

    m_YZenith = (4.0453 * m_turbitity - 4.9710) * tan( dChi ) - 0.2155 * m_turbitity + 2.4192;
//  m_YZenith *= 1000.0; // conversion from kcd/m^2 to cd/m^2

    if( m_YZenith < 0.0 )
        m_YZenith = -m_YZenith;

    m_xZenith = computeChromaticity( s_xZC );
    m_yZenith = computeChromaticity( s_yZC );

    // Calculate light scattered into view direction

    if( true == bInitA0 )
    {
        computeAttenuatedSunlight();

        int i, j;
        double theta, phi;

        double delTheta = g_dPI / c_thetaBins;
        double delPhi   = 2.0 * g_dPI / c_phiBins;

        for( i = 0, theta = 0; theta <= g_dPI; theta += delTheta, ++i )
            for( j = 0, phi = 0; phi <= 2.0 * g_dPI; phi+= delPhi,++j )
                calculateS0( theta, phi, m_S0_Mie[i * (c_thetaBins + 1) + j], m_S0_Ray[i * (c_thetaBins + 1) + j] );
    }
}

//-----------------------------------------------------------------------------
//  getNeta
//-----------------------------------------------------------------------------
void PreethamScatter::getNeta( const double a_theta, const double a_v, CVector3& a_neta ) const
{
    double theta = a_theta * g_dRadToDeg * 10.0;
    double u = theta - (int)theta;

    if( theta < 0.0 || theta > 1801.0 )
    {
        a_neta.SetVector( 0.0 );
        return;
    }

    if( theta > 1800 )
    {
        a_neta = m_netaTable[1800];
        return;
    }

    a_neta = (1.0f - (float)u) * m_netaTable[(int)theta] + (float)u * m_netaTable[(int)theta + 1];
}

//-----------------------------------------------------------------------------
//  calculateS0
//-----------------------------------------------------------------------------
void PreethamScatter::calculateS0( const double a_thetav, const double a_phiv, CVector3& a_S0_Mie, CVector3& a_S0_Ray ) const
{
    double phiDelta   = g_dPI / 20.0;
    double thetaDelta = g_dPI / 2.0 / 20.0;
    double thetaUpper = g_dPI / 2.0;
    double psi;

    CVector3 skyRad;
    CVector3 beta_ang_1, beta_ang_2;
    CVector3 neta;
    CVector3 skyAmb_1( 0.0, 0.0, 0.0 );
    CVector3 skyAmb_2( 0.0, 0.0, 0.0 );

    for( double theta = 0; theta < thetaUpper; theta += thetaDelta )
    {
      for( double phi = 0; phi < 2.0 * g_dPI; phi += phiDelta )
      {
          getSkySpectralRadiance( skyRad, theta, phi );
          psi = angleBetween( a_thetav, a_phiv, theta, phi );

          getNeta( psi, s_V, neta );
          beta_ang_1 = m_betaPAng * neta;
          beta_ang_2 = m_betaMAng * static_cast< float >( (1.0 + 0.9324 * cos( psi ) * cos( psi )) );

          skyAmb_1 += skyRad * beta_ang_1 * static_cast< float >( sin( theta ) * thetaDelta * phiDelta );
          skyAmb_2 += skyRad * beta_ang_2 * static_cast< float >( sin( theta ) * thetaDelta * phiDelta );
      }
    }

    psi = angleBetween( a_thetav, a_phiv, m_thetaSun, m_phiSun );

    getNeta( psi, s_V, neta );
    beta_ang_1 = m_betaPAng * neta;
    beta_ang_2 = m_betaMAng * static_cast< float >( (1.0 + 0.9324 * cos( psi ) * cos( psi )) );

    CVector3 sunAmb_1 = m_sunSpectralRad * beta_ang_1 * static_cast< float >( m_sunSolidAngle );
    CVector3 sunAmb_2 = m_sunSpectralRad * beta_ang_2 * static_cast< float >( m_sunSolidAngle );

    a_S0_Mie = sunAmb_1 + skyAmb_1;
    a_S0_Ray = sunAmb_2 + skyAmb_2;
}
