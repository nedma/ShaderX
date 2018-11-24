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

#include        "SkyDome.hpp"

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
SkyDome::SkyDome()
: m_pIScatter( NULL )
{}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
SkyDome::~SkyDome()
{
    destroy();
}

//-----------------------------------------------------------------------------
//  create
//-----------------------------------------------------------------------------
void SkyDome::create( const double a_radius
                    , const double a_height
                    , const int a_rings
                    , const int a_segments
                    )
{
    // Destroy current instance

    m_vertices.clear();
    m_indices .clear();

    // Clamp arguments

    double radius = Max( a_radius, g_dEpsilon );
    double height = Max( a_height, g_dEpsilon );

    int rings    = Max( a_rings, 1 ) + 1;
    int segments = Max( a_segments, 4 ) + 1;

    // Init vertex data container

    m_vertices.reserve( rings * (segments + 1) );

    // Fill vertex data container

    for( int y = 0; y <= segments; ++y )
    {
        double theta = static_cast< double >( y ) / segments * 360.0 * g_dDegToRad;

        for( int x = rings - 1; x >= 0; --x )
        {
            double phi = static_cast< double >( x ) / rings * 90.0 * g_dDegToRad;

            m_vertices.push_back( CVector3  ( static_cast< float >( sin( phi ) * cos( theta ) * radius )
                                            , static_cast< float >( sin( phi ) * sin( theta ) * radius )
                                            , static_cast< float >( cos( phi ) * height ) ) );
        }
    }

    // Init index container

    m_indices.reserve( segments * rings * 2 );

    // Fill index container

    bool leftToRight = false;

    for( y = 0; y < segments; ++y )
    {
        if( true == leftToRight )
        {
            for( int x = 0; x < rings; ++x )
            {
                m_indices.push_back( (y + 1) * rings + x );
                m_indices.push_back(  y      * rings + x );
            }
        }
        else
        {
            for( int x = rings - 1; x >= 0; --x )
            {
                m_indices.push_back(  y      * rings + x );
                m_indices.push_back( (y + 1) * rings + x );
            }
        }

        leftToRight = !leftToRight;
    }
}

//-----------------------------------------------------------------------------
//  destroy
//-----------------------------------------------------------------------------
void SkyDome::destroy()
{
    // Reset members

    m_vertices.clear();
    m_indices .clear();

    m_sunDirection.SetVector( 0.0f, 0.0f, 1.0f );

    m_pIScatter = NULL;
}

//-----------------------------------------------------------------------------
//  getVertices
//-----------------------------------------------------------------------------
const CVector3* SkyDome::getVertices() const
{
    return &m_vertices.front();
}

//-----------------------------------------------------------------------------
//  getIndices
//-----------------------------------------------------------------------------
const unsigned int* SkyDome::getIndices() const
{
    return &m_indices.front();
}

//-----------------------------------------------------------------------------
//  getIndexCount
//-----------------------------------------------------------------------------
const size_t SkyDome::getIndexCount() const
{
    return m_indices.size();
}
    
//-----------------------------------------------------------------------------
//  setSunDirection
//-----------------------------------------------------------------------------
void SkyDome::setSunDirection( const CVector3& a_direction )
{
    m_sunDirection = a_direction;
}

//-----------------------------------------------------------------------------
//  getSunDirection
//-----------------------------------------------------------------------------
const CVector3& SkyDome::getSunDirection() const
{
    return m_sunDirection;
}

//-----------------------------------------------------------------------------
//  setScatterClass
//-----------------------------------------------------------------------------
void SkyDome::setScatterClass( IScatter* a_pIScatterClass )
{
    m_pIScatter = a_pIScatterClass;
}

//-----------------------------------------------------------------------------
//  getScatterClass
//-----------------------------------------------------------------------------
IScatter* SkyDome::getScatterClass() const
{
    return m_pIScatter;
}
