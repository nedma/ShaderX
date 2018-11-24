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

#ifndef         PREETHAMSCATTER_H_INCLUDED
#define         PREETHAMSCATTER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "IScatter.hpp"
#include        <Cg/cg.h>
#include        "../math/CVector3.h"
#include        "../math/CVector4.h"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

static const int c_thetaBins = 5;
static const int c_phiBins   = 5;

//-----------------------------------------------------------------------------
//  PreethamScatter
//-----------------------------------------------------------------------------
//
//! Implements the scattering model as described by Preetham et al.
//
//-----------------------------------------------------------------------------
class PreethamScatter : public IScatter
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new PreethamScatter
                        PreethamScatter             ();
    //! \brief  Destructor
                        ~PreethamScatter            ();

    //! \brief  Initializes the Cg shader
    void                create                      ();

    //
    // Accessors
    //

    //! \brief  Sets the current turbiditiy
    //! \param  [in] The turbiditiy to set
    void                setTurbidity                ( const double a_turbidity );
    //! \brief  Sets the current scale factors
    //! \param  [in] The exposure factor to set
    //! \param  [in] The inscatter scale factor to set
    void                setScaleFactors             ( const float a_exposure
                                                    , const float a_scaleInscatter
                                                    );

    //
    // Interface IScatter implementation
    //

    //! \brief  Returns whether the skylight renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    const bool          isSkyLightValid             () const;
    //! \brief  Returns whether the aerial perspective renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    const bool          isAerialPerspectiveValid    () const;

    //! \brief  Renders skydome and skylight
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    //! \param  [in] The sky dome vertices
    //! \param  [in] The sky dome indices
    //! \param  [in] The sky dome index count
    void                renderSkyLight              ( const Camera& a_camera
                                                    , const CVector3& a_sunDirection
                                                    , const float* a_pVertices
                                                    , const unsigned int* a_pIndices
                                                    , const unsigned int a_indexCount
                                                    );

    //! \brief  Renders terrain and aerial perspective
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    //! \param  [in] The terrain vertices
    //! \param  [in] The terrain texture coordinates
    //! \param  [in] The terrain indices
    //! \param  [in] The terrain index count
    //! \param  [in] The terrain texture map
    //! \param  [in] The terrain normal map
    void                renderAerialPerspective     ( const Camera& a_camera
                                                    , const CVector3& a_sunDirection
                                                    , const float* a_pVertices
                                                    , const float* a_pTexCoords
                                                    , const unsigned int* a_pIndices
                                                    , const unsigned int a_indexCount
                                                    , const unsigned int a_terrainMap
                                                    , const unsigned int a_normalMap
                                                    );

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a PreethamScatter from the specified one
    //! \param  [in] The PreethamScatter to copy from
                        PreethamScatter             (const PreethamScatter&);
    //! \brief  Sets this PreethamScatter to the specified one
    //! \param  [in] The PreethamScatter to copy from
    //! \return The modified PreethamScatter
    PreethamScatter&    operator=                   (const PreethamScatter&);

    //
    // Skylight methods
    //

    //! \brief  Computes the chromaticity for the specified coefficients
    //! \param  [in] The chromaticity distribution coefficients to use
    //! \return The chromaticity
    const double        computeChromaticity         ( const double a_m[][4] ) const;

    //! \brief  Evaluates the Perez function
    //! \param  [in] The distribution coefficient for darkening or brightening of the horizon
    //! \param  [in] The distribution coefficient for the luminance gradient near the horizon
    //! \param  [in] The distribution coefficient for the relative intensity of the circumsolar region
    //! \param  [in] The distribution coefficient for the width of the circumsolar region
    //! \param  [in] The distribution coefficient for relative backscattered light
    //! \param  [in] The angle between sun direction and view direction 
    //! \param  [in] The angle between zenith and view direction
    //! \return The sky luminance/chromaticity distribution
    const double        perezFunction               ( const double a_A
                                                    , const double a_B
                                                    , const double a_C
                                                    , const double a_D
                                                    , const double a_E
                                                    , const double a_theta
                                                    , const double a_gamma
                                                    ) const;

    //! \brief  Computes the sun's spectral radiance for the current sun position
    void                computeAttenuatedSunlight   ();

    //! \brief  Computes the sky's spectral radiance
    //! \param  [out] The spectral radiance converted to CIE XYZ
    //! \param  [in] The point of interest's angle from zenith
    //! \param  [in] The point of interest's horizontal angle
    void                getSkySpectralRadiance      ( CVector3& a_color
                                                    , const double a_theta
                                                    , const double a_phi
                                                    ) const;

    //! \brief  Sets the current sun position
    //! \param  [in] The sun's angle from zenith
    //! \param  [in] The sun's horizontal angle
    void                setSun                      ( const double a_theta
                                                    , const double a_phi
                                                    );

    //
    // Aerial perspective methods
    //

    //! \brief  Returns the neta value for the specified parameter
    //! \param  [in] The angle between sun direction and view direction 
    //! \param  [in] Junge's exponent
    //! \param  [out] The corresponding value for neta
    void                getNeta                     ( const double a_theta
                                                    , const double a_v
                                                    , CVector3& a_neta
                                                    ) const;

    //! \brief  Calculates the light scattered into the view direction 
    //! \param  [in] The view directions's angle from zenith
    //! \param  [in] The The view directions's horizontal angle
    //! \param  [out] The light scattered for Mie particles in as CIE XYZ
    //! \param  [out] The light scattered for Rayleigh particles in as CIE XYZ
    void                calculateS0                 ( const double a_thetav
                                                    , const double a_phiv
                                                    , CVector3& a_S0_Mie
                                                    , CVector3& a_S0_Ray
                                                    ) const;

    //
    // Distribution tables
    //

    static const double s_xDC[5][2];  //!< Chromaticity distribution coefficients
    static const double s_yDC[5][2];  //!< Chromaticity distribution coefficients
    static const double s_YDC[5][2];  //!< Luminance distribution coefficients
    static const double s_xZC[3][4];  //!< Chromaticity distribution coefficients for zenith
    static const double s_yZC[3][4];  //!< Chromaticity distribution coefficients for zenith

    //
    // Color conversion tables
    //

    static const double s_CM[9];                //!< Rec. 709 color conversion matrix
    static const double s_CIE_300_800[107][3];  //!< CIE 1931 2-deg, XYZ Color Matching Functions (300nm - 800nm)
    static const double s_CIE_350_800[97][3];   //!< CIE 1931 2-deg, XYZ Color Matching Functions (350nm - 800nm)

    //
    // Static aerial perspective tables
    //

    static const double s_neta[];
    static const double s_K[];
    static const double s_N21[];
    static const double s_Ko[];
    static const double s_Kg[];
    static const double s_Kwa[];
    static const double s_sol[];

    //
    // Aerial perspective constants
    //

    static const double s_V;
    static const double s_anisCor;
    static const double s_N;
    static const double s_alpha1;
    static const double s_alpha2;

    //
    // Dynamic aerial perspective tables
    //

    CVector3 m_netaTable[ 1801 ];
    CVector3 m_S0_Mie[ (c_thetaBins + 1) * (c_phiBins + 1) ];  //!< Light scattered into the view direction for Mie particles
    CVector3 m_S0_Ray[ (c_thetaBins + 1) * (c_phiBins + 1) ];  //!< Light scattered into the view direction for Rayleigh particles

    //
    // Atmosphere data
    //

    double m_Ax, m_Bx, m_Cx, m_Dx, m_Ex;    //!< Perez function parameter for chromaticity
    double m_Ay, m_By, m_Cy, m_Dy, m_Ey;    //!< Perez function parameter for chromaticity
    double m_AY, m_BY, m_CY, m_DY, m_EY;    //!< Perez function parameter for luminance

    double m_YZenith;   //!< Zenith luminance
    double m_xZenith;   //!< Zenith chromaticity
    double m_yZenith;   //!< Zenith chromaticity

    double m_turbitity; //!< Current turbidity

    //
    // Sun data
    //

    double m_sunSolidAngle; //!< Sun's solid angle w
    double m_thetaSun;      //!< Sun's angle from zenith
    double m_phiSun;        //!< Sun's horizontal angle

    //
    // Aerial perspective data
    //

    CVector3 m_sunSpectralRad;  //!< Sun's spectral radiance
    CVector3 m_betaM;           //!< Rayleigh total scattering coefficient
    CVector3 m_betaP;           //!< Mie total scattering coefficient
    CVector3 m_betaMAng;        //!< Rayleigh angular scattering coefficient
    CVector3 m_betaPAng;        //!< Mie angular scattering coefficient
    CVector4 m_constants;       //!< Shader constants

    //
    // Skylight Cg data
    //

    static char         s_skyVertexShaderSource[];   //!< Sky vertex shader source
    static char         s_skyFragmentShaderSource[]; //!< Sky fragment shader source

    CGprogram           m_cgSkyVertexShader;    //!< Sky vertex shader
    CGprogram           m_cgSkyFragmentShader;  //!< Sky fragment shader

    CGparameter         m_cgSkyPosition;    //!< Sky vertex shader position parameter
    CGparameter         m_cgSkySunDir;      //!< Sky vertex shader sun direction parameter
    CGparameter         m_cgSkyThetaS;      //!< Sky vertex shader sun's angle from zenith parameter
    CGparameter         m_cgSkyZenith;      //!< Sky vertex shader zenith direction parameter
    CGparameter         m_cgSkyZenith_xyY;  //!< Sky vertex shader zenith chromaticity and luminance parameter
    CGparameter         m_cgSkyA_xyY;       //!< Sky vertex shader Perez function parameter
    CGparameter         m_cgSkyB_xyY;       //!< Sky vertex shader Perez function parameter
    CGparameter         m_cgSkyC_xyY;       //!< Sky vertex shader Perez function parameter
    CGparameter         m_cgSkyD_xyY;       //!< Sky vertex shader Perez function parameter
    CGparameter         m_cgSkyE_xyY;       //!< Sky vertex shader Perez function parameter
    CGparameter         m_cgSkyCM;          //!< Sky vertex shader color conversion matrix parameter
    CGparameter         m_cgSkyMVP;         //!< Sky vertex shader model/view/projection matrix parameter
    CGparameter         m_cgSkyM;           //!< Sky vertex shader model matrix parameter
    CGparameter         m_cgSkyExposure;    //!< Sky vertex shader exposure parameter

    //
    // Aerial perspective Cg data
    //

    static char         s_terrainVertexShaderSource[];   //!< Terrain vertex shader source
    static char         s_terrainFragmentShaderSource[]; //!< Terrain fragment shader source

    CGprogram           m_cgTerrainVertexShader;    //!< Terrain vertex shader
    CGprogram           m_cgTerrainFragmentShader;  //!< Terrain fragment shader

    CGparameter         m_cgTerrainPosition;    //!< Terrain vertex shader position parameter
    CGparameter         m_cgTerrainTexCoord;    //!< Terrain vertex shader texture coordinates parameter
    CGparameter         m_cgTerrainEyePos;      //!< Terrain vertex shader eye position parameter
    CGparameter         m_cgTerrainBetaMie;     //!< Terrain vertex shader Mie scattering coefficient parameter
    CGparameter         m_cgTerrainBetaRay;     //!< Terrain vertex shader Rayleigh scattering coefficient parameter
    CGparameter         m_cgTerrainAlpha;       //!< Terrain vertex shader alpha constants parameter
    CGparameter         m_cgTerrainBins;        //!< Terrain vertex shader bin constants parameter
    CGparameter         m_cgTerrainConstants;   //!< Terrain vertex shader constant parameter
    CGparameter         m_cgTerrainC1plusC2;    //!< Terrain vertex shader constant parameter
    CGparameter         m_cgTerrainCM;          //!< Terrain vertex shader color conversion matrix  parameter
    CGparameter         m_cgTerrainMVP;         //!< Terrain vertex shader model/view/projection matrix  parameter
    CGparameter         m_cgTerrainMV;          //!< Terrain vertex shader model/view matrix  parameter
    CGparameter         m_cgTerrainS0_Mie;      //!< Terrain vertex shader S0 for Mie parameter
    CGparameter         m_cgTerrainS0_Ray;      //!< Terrain vertex shader S0 for Rayleigh parameter

    CGparameter         m_cgTerrainLightVector; //!< Terrain fragment shader light vector parameter
    CGparameter         m_cgTerrainMap;         //!< Terrain fragment shader texture map parameter
    CGparameter         m_cgTerrainNormalMap;   //!< Terrain fragment shader normal map parameter

    bool                m_validSkyLight;          //!< Flag indicating whether the skylight shader is loadable
    bool                m_validAerialPerspective; //!< Flag indicating whether the airlight shader is loadable

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // PREETHAMSCATTER_H_INCLUDED
