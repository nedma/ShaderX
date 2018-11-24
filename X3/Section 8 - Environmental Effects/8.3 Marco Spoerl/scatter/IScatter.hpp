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

#ifndef         ISCATTER_H_INCLUDED
#define         ISCATTER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

class           Camera;
class           CVector3;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  IScatter
//-----------------------------------------------------------------------------
//
//! Interface for rendering skylight and aerial perspective
//
//-----------------------------------------------------------------------------
class IScatter
{

public:

    //! \brief  Returns whether the skylight renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    virtual const bool  isSkyLightValid         () const = 0;

    //! \brief  Returns whether the aerial perspective renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    virtual const bool  isAerialPerspectiveValid() const = 0;

    //! \brief  Renders skydome and skylight
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    //! \param  [in] The sky dome vertices
    //! \param  [in] The sky dome indices
    //! \param  [in] The sky dome index count
    virtual void        renderSkyLight          ( const Camera& a_camera
                                                , const CVector3& a_sunDirection
                                                , const float* a_pVertices
                                                , const unsigned int* a_pIndices
                                                , const unsigned int a_indexCount
                                                ) = 0;

    //! \brief  Renders terrain and aerial perspective
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    //! \param  [in] The terrain vertices
    //! \param  [in] The terrain texture coordinates
    //! \param  [in] The terrain indices
    //! \param  [in] The terrain index count
    //! \param  [in] The terrain texture map
    //! \param  [in] The terrain normal map
    virtual void        renderAerialPerspective ( const Camera& a_camera
                                                , const CVector3& a_sunDirection
                                                , const float* a_pVertices
                                                , const float* a_pTexCoords
                                                , const unsigned int* a_pIndices
                                                , const unsigned int a_indexCount
                                                , const unsigned int a_terrainMap
                                                , const unsigned int a_normalMap
                                                ) = 0;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // ISCATTER_H_INCLUDED
