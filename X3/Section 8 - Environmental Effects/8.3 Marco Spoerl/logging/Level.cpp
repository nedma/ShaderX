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

#include        "Level.hpp"

using namespace Logging;

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
Level::Level( const std::string& a_name, const int a_value )
: m_name( a_name ), m_value( a_value )
{
}

//-----------------------------------------------------------------------------
//  Copy constructor
//-----------------------------------------------------------------------------
Level::Level( const Level& a_other )
{
    *this = a_other;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
Level::~Level()
{
}

//-----------------------------------------------------------------------------
//  getName
//-----------------------------------------------------------------------------
const std::string& Level::getName() const
{
    return m_name;
}

//-----------------------------------------------------------------------------
//  getValue
//-----------------------------------------------------------------------------
const int Level::getValue() const
{
    return m_value;
}
    
//-----------------------------------------------------------------------------
//  Copy assignment operator
//-----------------------------------------------------------------------------
const Level& Level::operator =( const Level& a_other )
{
    m_name  = a_other.getName();
    m_value = a_other.getValue();

    return *this;
}
