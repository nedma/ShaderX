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

#include        "SimpleFormatter.hpp"
#include        "LogRecord.hpp"
#include        <ctime>

using namespace Logging;

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
SimpleFormatter::SimpleFormatter()
{
}

//-----------------------------------------------------------------------------
//  Copy constructor
//-----------------------------------------------------------------------------
SimpleFormatter::SimpleFormatter( const SimpleFormatter& a_other )
{
    *this = a_other;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
SimpleFormatter::~SimpleFormatter()
{
}

//-----------------------------------------------------------------------------
//  format
//-----------------------------------------------------------------------------
const std::string SimpleFormatter::format( const LogRecord& a_record )
{
    // Convert time value to time string

    time_t      ltime = a_record.getTime();
    tm*         pTime = localtime( &ltime );
    std::string time  ( asctime( pTime ) );

    // Compose message string. Format is:
    // Date/time - level - raw message

    std::string message = time.substr( 0, time.length() - 1 ) + " - " + a_record.getLevel().getName() + " - " + a_record.getMessage() + "\n";

    return message;
}

//-----------------------------------------------------------------------------
//  clone
//-----------------------------------------------------------------------------
IFormatter* SimpleFormatter::clone() const
{
    return new SimpleFormatter( *this );
}

//-----------------------------------------------------------------------------
//  Copy assignment operator
//-----------------------------------------------------------------------------
const SimpleFormatter& SimpleFormatter::operator =( const SimpleFormatter& a_other )
{
    return *this;
}
