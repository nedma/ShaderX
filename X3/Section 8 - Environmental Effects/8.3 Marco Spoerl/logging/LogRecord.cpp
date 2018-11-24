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

#include        "LogRecord.hpp"
#include        <ctime>

using namespace Logging;

//*************************************************************************************************
//*************************************************************************************************
//**    Static initialization
//*************************************************************************************************
//*************************************************************************************************

long LogRecord::s_nextSequenceNumber = 0L;

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------------------
LogRecord::LogRecord( const Level& a_level, const std::string& a_message )
: m_level( a_level ), m_message( a_message )
{
    time( &m_time );
    m_sequenceNumber = s_nextSequenceNumber++;
}

//-----------------------------------------------------------------------------
//  Copy constructor
//-----------------------------------------------------------------------------
LogRecord::LogRecord( const LogRecord& a_other )
: m_level( a_other.getLevel() )
{
    *this = a_other;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
LogRecord::~LogRecord()
{
}

//-----------------------------------------------------------------------------
//  getLevel
//-----------------------------------------------------------------------------
const Level& LogRecord::getLevel() const
{
    return m_level;
}

//-----------------------------------------------------------------------------
//  getMessage
//-----------------------------------------------------------------------------
const std::string& LogRecord::getMessage() const
{
    return m_message;
}

//-----------------------------------------------------------------------------
//  getTime
//-----------------------------------------------------------------------------
const long LogRecord::getTime() const
{
    return m_time;
}

//-----------------------------------------------------------------------------
//  getSequenceNumber
//-----------------------------------------------------------------------------
const long LogRecord::getSequenceNumber() const
{
    return m_sequenceNumber;
}

//-----------------------------------------------------------------------------
//  setLevel
//-----------------------------------------------------------------------------
void LogRecord::setLevel( const Level& a_level )
{
    m_level = a_level;
}

//-----------------------------------------------------------------------------
//  setMessage
//-----------------------------------------------------------------------------
void LogRecord::setMessage( const std::string& a_message )
{
    m_message = a_message;
}

//-----------------------------------------------------------------------------
//  setTime
//-----------------------------------------------------------------------------
void LogRecord::setTime( const time_t a_time )
{
    m_time = a_time;
}

//-----------------------------------------------------------------------------
//  setSequenceNumber
//-----------------------------------------------------------------------------
void LogRecord::setSequenceNumber( const long a_sequenceNumber )
{
    m_sequenceNumber = a_sequenceNumber;
}

//-----------------------------------------------------------------------------
//  Copy assignment operator
//-----------------------------------------------------------------------------
const LogRecord& LogRecord::operator =( const LogRecord& a_other )
{
    m_level          = a_other.getLevel();
    m_message        = a_other.getMessage();
    m_time           = a_other.getTime();
    m_sequenceNumber = a_other.getSequenceNumber();

    return *this;
}
