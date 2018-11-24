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

#ifndef         SKYDOME_H_INCLUDED
#define         SKYDOME_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <vector>
#include        "../math/CVector3.h"

class           IScatter;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  SkyDome
//-----------------------------------------------------------------------------
//
//! Sky dome hemisphere and associated scatter class
//
//-----------------------------------------------------------------------------
class SkyDome
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new sky dome
                        SkyDome         ();
    //! \brief  Destructor
                        ~SkyDome        ();

    //! \brief  Creates the sky dome from the given parameter
    //! \param  [in] The hemisphere radius
    //! \param  [in] The hemisphere height
    //! \param  [in] The number of rings for the hemisphere
    //! \param  [in] The number of segmants for the hemisphere
    void                create          ( const double a_radius
                                        , const double a_height
                                        , const int a_rings
                                        , const int a_segments
                                        );
    //! \brief  Destroys the sky dome
    void                destroy         ();

    //
    // Accessors
    //

    //! \brief  Returns the sky dome vertices
    //! \return The sky dome vertices
    const CVector3*     getVertices     () const;
    //! \brief  Returns the sky dome triangle strip indices
    //! \return The sky dome indices
    const unsigned int* getIndices      () const;
    //! \brief  Returns the sky dome triangle strip index count
    //! \return The index count
    const size_t        getIndexCount   () const;

    //! \brief  Sets the current sun direction
    //! \param  [in] The sun direction to set
    void                setSunDirection ( const CVector3& a_direction );
    //! \brief  Returns the current sun direction
    //! \return The current sun direction
    const CVector3&     getSunDirection () const;

    //! \brief  Sets the current scatter class
    //! \param  [in] The scatter class to set
    void                setScatterClass ( IScatter* a_pIScatterClass );
    //! \brief  Returns the current scatter class
    //! \return The current scatter class
    IScatter*           getScatterClass () const;

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a SkyDome from the specified one
    //! \param  [in] The SkyDome to copy from
                        SkyDome         ( const SkyDome& );
    //! \brief  Sets this SkyDome to the specified one
    //! \param  [in] The SkyDome to copy from
    //! \return The modified SkyDome
    SkyDome&            operator=       ( const SkyDome& );

    //
    // Dome data
    //

    std::vector< CVector3 >     m_vertices;     //!< Hemisphere vertices
    std::vector< unsigned int > m_indices;      //!< Hemisphere indices
    CVector3                    m_sunDirection; //!< Current sun direction
    IScatter*                   m_pIScatter;    //!< Scatter class for rendering skylight and aerial perspective

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // SKYDOME_H_INCLUDED
