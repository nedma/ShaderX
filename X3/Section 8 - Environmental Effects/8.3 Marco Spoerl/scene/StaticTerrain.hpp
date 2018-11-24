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

#ifndef         STATICTERRAIN_H_INCLUDED
#define         STATICTERRAIN_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <vector>

class           HeightField;
class           CVector3;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  StaticTerrain
//-----------------------------------------------------------------------------
//
//! Basic static landscape mesh
//
//-----------------------------------------------------------------------------
class StaticTerrain
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new empty landscape
                        StaticTerrain   ();
    //! \brief  Destructor
                        ~StaticTerrain  ();

    //! \brief  Creates the landscape from the given height field
    //! \param  [in] The height field to use
    void                create          ( const HeightField& a_heightField );
    //! \brief  Destroys the landscape
    void                destroy         ();

    //
    // Accessors
    //

    //! \brief  Returns the landscape vertices
    //! \return The landscape vertices
    const CVector3*     getVertices     () const;
    //! \brief  Returns the landscape texture coordinates
    //! \return The landscape texture coordinates
    const CVector3*     getTexCoords    () const;
    //! \brief  Returns the landscape triangle strip indices
    //! \return The landscape indices
    const unsigned int* getIndices      () const;

    //! \brief  Returns the landscape triangle strip index count
    //! \return The index count
    const size_t        getIndexCount   () const;
    //! \brief  Returns the landscape total width
    //! \return The total width
    const double        getTotalWidth   () const;
    //! \brief  Returns the landscape total height
    //! \return The total height
    const double        getTotalHeight  () const;

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a StaticTerrain from the specified one
    //! \param  [in] The StaticTerrain to copy from
                        StaticTerrain   ( const StaticTerrain& );
    //! \brief  Sets this StaticTerrain to the specified one
    //! \param  [in] The StaticTerrain to copy from
    //! \return The modified StaticTerrain
    StaticTerrain&      operator=       ( const StaticTerrain& );

    //
    // Terrain data
    //

    std::vector< CVector3 >     m_vertices;  //!< Terrain vertices
    std::vector< CVector3 >     m_texCoords; //!< Terrain texture coordinates
    std::vector< unsigned int > m_indices;   //!< Terrain indices

    double m_totalWidth;    // Total x-dimension of the terrain
    double m_totalHeight;   // Total y-dimension of the terrain

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // STATICTERRAIN_H_INCLUDED
