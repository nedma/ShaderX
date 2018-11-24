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

#ifndef         HOFFMANSCATTER_H_INCLUDED
#define         HOFFMANSCATTER_H_INCLUDED

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

//-----------------------------------------------------------------------------
//  HoffmanScatter
//-----------------------------------------------------------------------------
//
//! Implements the scattering model as described by Hoffman et al.
//
//-----------------------------------------------------------------------------
class HoffmanScatter : public IScatter
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new HoffmanScatter
                    HoffmanScatter          ();
    //! \brief  Destructor
                    ~HoffmanScatter         ();

    //! \brief  Initializes the Cg shader
    void            create                  ();

    //
    // Accessors
    //

    //! \brief  Sets the current sun intensity
    //! \param  [in] The sun intensity to set
    void            setSunIntensity         ( const float a_value );
    //! \brief  Sets the current Mie scattering coefficient
    //! \param  [in] The Mie scattering coefficient to set
    void            setMie                  ( const float a_value );
    //! \brief  Sets the current Rayleigh scattering coefficient
    //! \param  [in] The Rayleigh scattering coefficient to set
    void            setRayleigh             ( const float a_value );
    //! \brief  Sets the current Henyey Greenstein g-value
    //! \param  [in] The Henyey Greenstein g-value to set
    void            setHg                   ( const float a_value );
    //! \brief  Sets the current inscattering factor
    //! \param  [in] The inscattering factor to set
    void            setInscattering         ( const float a_value );
    //! \brief  Sets the current turbiditiy
    //! \param  [in] The turbiditiy to set
    void            setTurbidity            ( const float a_value );

    //
    // Interface IScatter implementation
    //

    //! \brief  Returns whether the skylight renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    const bool      isSkyLightValid         () const;
    //! \brief  Returns whether the aerial perspective renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    const bool      isAerialPerspectiveValid() const;

    //! \brief  Renders skydome and skylight
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    //! \param  [in] The sky dome vertices
    //! \param  [in] The sky dome indices
    //! \param  [in] The sky dome index count
    void            renderSkyLight          ( const Camera& a_camera
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
    void            renderAerialPerspective ( const Camera& a_camera
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

    //! \brief  Creates a HoffmanScatter from the specified one
    //! \param  [in] The HoffmanScatter to copy from
                    HoffmanScatter          ( const HoffmanScatter& );
    //! \brief  Sets this HoffmanScatter to the specified one
    //! \param  [in] The HoffmanScatter to copy from
    //! \return The modified HoffmanScatter
    HoffmanScatter& operator=               ( const HoffmanScatter& );

    //
    // Helper
    //

    //! \brief  Computes the sun attenuation for the specified sun angle
    //! \param  [in] The sun angle to use
    void            computeAttenuation      ( const float a_theta );
    //! \brief  Updates the shader constants
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    void            setShaderConstants      ( const Camera& a_camera
                                            , const CVector3& a_sunDirection
                                            );

    //
    // Atmosphere data
    //

    float       m_HGg;                      //!< g value in Henyey Greenstein approximation function
    float       m_inscatteringMultiplier;   //!< Multiply inscattering term with this factor
    float       m_betaRayMultiplier;        //!< Multiply Rayleigh scattering coefficient with this factor
    float       m_betaMieMultiplier;        //!< Multiply Mie scattering coefficient with this factor
    float       m_sunIntensity;             //!< Current sun intensity
    float       m_turbitity;                //!< Current turbidity

    CVector3    m_betaRay;                  //!< Rayleigh total scattering coefficient
    CVector3    m_betaDashRay;              //!< Rayleigh angular scattering coefficient without phase term
    CVector3    m_betaMie;                  //!< Mie total scattering coefficient
    CVector3    m_betaDashMie;              //!< Mie angular scattering coefficient without phase term
    CVector4    m_sunColorAndIntensity;     //!< Sun color and intensity packed for the shader

    //
    // Cg data
    //

    static char s_vertexShaderSource[];            //!< Shared Hoffman vertex shader source
    static char s_skyFragmentShaderSource[];       //!< Sky fragment shader source
    static char s_terrainFragmentShaderSource[];   //!< Terrain fragment shader source

    CGprogram   m_cgVertexShader;           //!< Shared Hoffman vertex shader
    CGprogram   m_cgSkyFragmentShader;      //!< Sky fragment shader
    CGprogram   m_cgTerrainFragmentShader;  //!< Terrain fragment shader

    CGparameter m_cgPosition;               //!< Shared vertex shader position parameter
    CGparameter m_cgTexCoord;               //!< Shared vertex shader texture coordinate parameter
    CGparameter m_cgEyePos;                 //!< Shared vertex shader eye position parameter
    CGparameter m_cgSunDir;                 //!< Shared vertex shader sun direction parameter
    CGparameter m_cgBetaRPlusBetaM;         //!< Shared vertex shader scattering coefficients parameter
    CGparameter m_cgHGg;                    //!< Shared vertex shader Henyey Greenstein parameter
    CGparameter m_cgBetaDashR;              //!< Shared vertex shader scattering coefficients parameter
    CGparameter m_cgBetaDashM;              //!< Shared vertex shader scattering coefficients parameter
    CGparameter m_cgOneOverBetaRPlusBetaM;  //!< Shared vertex shader scattering coefficients parameter
    CGparameter m_cgMultipliers;            //!< Shared vertex shader multiplier parameter
    CGparameter m_cgSunColorAndIntensity;   //!< Shared vertex shader sun color/intensity parameter
    CGparameter m_cgMVP;                    //!< Shared vertex shader model/view/projection matrix parameter
    CGparameter m_cgMV;                     //!< Shared vertex shader model/view matrix parameter

    CGparameter m_cgLightVector;            //!< Terrain fragment shader light vector parameter
    CGparameter m_cgTerrainMap;             //!< Terrain fragment shader texture map parameter
    CGparameter m_cgNormalMap;              //!< Terrain fragment shader normal map parameter

    bool        m_validSkyLight;            //!< Flag indicating whether the skylight shader is loadable
    bool        m_validAerialPerspective;   //!< Flag indicating whether the airlight shader is loadable

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // HOFFMANSCATTER_H_INCLUDED
