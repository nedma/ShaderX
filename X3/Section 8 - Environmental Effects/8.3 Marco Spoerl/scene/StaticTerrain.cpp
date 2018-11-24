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

#include        "StaticTerrain.hpp"
#include        "HeightField.hpp"
#include        "../math/CVector3.h"

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
StaticTerrain::StaticTerrain()
: m_totalWidth( 0.0 ), m_totalHeight( 0.0 )
{}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
StaticTerrain::~StaticTerrain()
{
    destroy();
}

//-----------------------------------------------------------------------------
//  create
//-----------------------------------------------------------------------------
void StaticTerrain::create( const HeightField& a_heightField )
{
    // Destroy current instance

    destroy();

    // Init vertex and normal data container

    unsigned int vertexCount = a_heightField.getFieldWidth() * a_heightField.getFieldHeight();

    m_vertices .reserve( vertexCount );
    m_texCoords.reserve( vertexCount );

    // Fill vertex and texcoord data container

    for( unsigned int y = 0; y < a_heightField.getFieldHeight(); ++y )
    {
        for( unsigned int x = 0; x < a_heightField.getFieldWidth(); ++x )
        {
            m_vertices.push_back( CVector3  ( static_cast< float >( x * a_heightField.getGridSpacing() )
                                            , static_cast< float >( y * a_heightField.getGridSpacing() )
                                            , static_cast< float >( a_heightField.getHeight( x, y ) ) ) );

            m_texCoords.push_back( CVector3 ( static_cast< float >( x ) / a_heightField.getFieldWidth()
                                            , static_cast< float >( y ) / a_heightField.getFieldHeight()
                                            , 0.0 ) );
        }
    }

    // Init index container

    m_indices.reserve( (a_heightField.getFieldHeight() - 1) * a_heightField.getFieldWidth() * 2 );

    // Fill index container

    bool leftToRight = true;

    for( y = 0; y < a_heightField.getFieldHeight() - 1; ++y )
    {
        if( true == leftToRight )
        {
            for( unsigned int x = 0; x < a_heightField.getFieldWidth(); ++x )
            {
                m_indices.push_back( (y + 1) * a_heightField.getFieldWidth() + x );
                m_indices.push_back(  y      * a_heightField.getFieldWidth() + x );
            }
        }
        else
        {
            for( int x = a_heightField.getFieldWidth() - 1; x >= 0; --x )
            {
                m_indices.push_back(  y      * a_heightField.getFieldWidth() + x );
                m_indices.push_back( (y + 1) * a_heightField.getFieldWidth() + x );
            }
        }

        leftToRight = !leftToRight;
    }

    // Set terrain dimensions

    m_totalWidth  = a_heightField.getFieldWidth() * a_heightField.getGridSpacing();
    m_totalHeight = a_heightField.getFieldHeight() * a_heightField.getGridSpacing();
}

//-----------------------------------------------------------------------------
//  destroy
//-----------------------------------------------------------------------------
void StaticTerrain::destroy()
{
    // Reset members

    m_vertices .clear();
    m_texCoords.clear();
    m_indices  .clear();

    m_totalWidth  = 0.0;
    m_totalHeight = 0.0;
}

//-----------------------------------------------------------------------------
//  getVertices
//-----------------------------------------------------------------------------
const CVector3* StaticTerrain::getVertices() const
{
    return &m_vertices.front();
}

//-----------------------------------------------------------------------------
//  getTexCoords
//-----------------------------------------------------------------------------
const CVector3* StaticTerrain::getTexCoords() const
{
    return &m_texCoords.front();
}

//-----------------------------------------------------------------------------
//  getIndices
//-----------------------------------------------------------------------------
const unsigned int* StaticTerrain::getIndices() const
{
    return &m_indices.front();
}

//-----------------------------------------------------------------------------
//  getIndexCount
//-----------------------------------------------------------------------------
const size_t StaticTerrain::getIndexCount() const
{
    return m_indices.size();
}

//-----------------------------------------------------------------------------
//  getTotalWidth
//-----------------------------------------------------------------------------
const double StaticTerrain::getTotalWidth() const
{
    return m_totalWidth;
}

//-----------------------------------------------------------------------------
//  getTotalHeight
//-----------------------------------------------------------------------------
const double StaticTerrain::getTotalHeight() const
{
    return m_totalHeight;
}
