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

#ifndef         TERRAINRENDERER_H_INCLUDED
#define         TERRAINRENDERER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "IRenderer.hpp"
#include        <Cg/cg.h>

class           StaticTerrain;
class           SkyDome;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  TerrainRenderer
//-----------------------------------------------------------------------------
//
//! Basic static terrain renderer
//
//-----------------------------------------------------------------------------
class TerrainRenderer : public IRenderer
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new TerrainRenderer
    //! \param  [in] The terrain to render
    //! \param  [in] The accompanying sky
    //! \param  [in] The RGB image data to use as terrain texture
    //! \param  [in] The terrain texture image width
    //! \param  [in] The terrain texture image height
    //! \param  [in] The RGB image data to use as terrain normal map
    //! \param  [in] The normal map image width
    //! \param  [in] The normal map image height
                        TerrainRenderer ( const StaticTerrain* a_pTerrain
                                        , const SkyDome* a_pSkyDome
                                        , const unsigned char* a_terrainMap
                                        , const int a_terrainMapWidth
                                        , const int a_terrainMapHeight
                                        , const unsigned char* a_normalMap
                                        , const int a_normalMapWidth
                                        , const int a_normalMapHeight
                                        );
    //! \brief  Destructor
                        ~TerrainRenderer();

    //
    // Interface IRender implementation
    //

    //! \brief  Renders the terrain
    //! \param  [in] The camera to use
    //! \param  [in] Flag indicating whether to render in wireframe mode
    void                render          ( const Camera& a_camera, const bool a_wireframe );

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a TerrainRenderer from the specified one
    //! \param  [in] The TerrainRenderer to copy from
                        TerrainRenderer ( const TerrainRenderer& );
    //! \brief  Sets this TerrainRenderer to the specified one
    //! \param  [in] The TerrainRenderer to copy from
    //! \return The modified TerrainRenderer
    TerrainRenderer&    operator=       ( const TerrainRenderer& );

    //
    // TerrainRenderer data
    //

    const StaticTerrain*    m_pTerrain;                 //!< The terrain to render
    const SkyDome*          m_pSkyDome;                 //!< The accompanying sky

    unsigned int            m_terrainMap;               //!< The terrain texture map
    unsigned int            m_normalMap;                //!< The terrain normal map

    static char             s_vertexShaderSource[];     //!< Basic sky vertex shader source
    static char             s_fragmentShaderSource[];   //!< Basic sky fragment shader source

    CGprogram               m_cgVertexShader;           //!< Basic sky vertex shader
    CGprogram               m_cgFragmentShader;         //!< Basic sky fragment shader

    CGparameter             m_cgPosition;               //!< Terrain vertex shader position parameter
    CGparameter             m_cgTexCoord;               //!< Terrain vertex shader texture coordinate parameter
    CGparameter             m_cgMVP;                    //!< Terrain vertex shader model/view/projection matrix parameter

    CGparameter             m_cgLightVector;            //!< Terrain fragment shader light vector parameter
    CGparameter             m_cgTerrainMap;             //!< Terrain fragment shader texture map parameter
    CGparameter             m_cgNormalMap;              //!< Terrain fragment shader normal map parameter

    bool                    m_shaderValid;              //!< Flag indicating whether the default vertex and fragment programs are valid

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // TERRAINRENDERER_H_INCLUDED
