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

#include        "ConsoleHandler.hpp"
#include        "IFormatter.hpp"
#include        <iostream>

using namespace Logging;

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
ConsoleHandler::ConsoleHandler()
{
}

//-----------------------------------------------------------------------------
//  Copy constructor
//-----------------------------------------------------------------------------
ConsoleHandler::ConsoleHandler( const ConsoleHandler& a_other )
: HandlerBase( a_other )
{
    *this = a_other;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
ConsoleHandler::~ConsoleHandler()
{
}

//-----------------------------------------------------------------------------
//  publish
//-----------------------------------------------------------------------------
void ConsoleHandler::publish( const LogRecord& a_record )
{
    // Publish record at all?

    if( false == isLoggable( a_record ) )
        return;

    // Write formatted message to cerr

    std::cerr << m_pFormatter->format( a_record ).c_str();
}

//-----------------------------------------------------------------------------
//  clone
//-----------------------------------------------------------------------------
IHandler* ConsoleHandler::clone() const
{
    return new ConsoleHandler( *this );
}

//-----------------------------------------------------------------------------
//  Copy assignment operator
//-----------------------------------------------------------------------------
const ConsoleHandler& ConsoleHandler::operator =( const ConsoleHandler& a_other )
{
    return *this;
}
