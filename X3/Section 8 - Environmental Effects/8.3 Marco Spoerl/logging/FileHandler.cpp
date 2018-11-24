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

#include        "FileHandler.hpp"
#include        "IFormatter.hpp"
#include        "../math/Basics.h"
#include        <sstream>

using namespace Logging;

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
FileHandler::FileHandler()
: m_fileName(), m_limit( 0 ), m_count( 1 ), m_append( false ), m_currentFile( 1 )
{
    open();
}

//-----------------------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------------------
FileHandler::FileHandler( const std::string& a_fileName )
: m_fileName( a_fileName ), m_limit( 0 ), m_count( 1 ), m_append( false ), m_currentFile( 1 )
{
    open();
}

//-----------------------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------------------
FileHandler::FileHandler( const std::string& a_fileName
                        , const bool a_append
                        )
: m_fileName( a_fileName ), m_limit( 0 ), m_count( 1 ), m_append( a_append ), m_currentFile( 1 )
{
    open();
}

//-----------------------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------------------
FileHandler::FileHandler( const std::string& a_fileName
                        , const int a_limit
                        , const int a_count
                        )
: m_fileName( a_fileName ), m_limit( a_limit ), m_count( a_count ), m_append( false ), m_currentFile( 1 )
{
    open();
}

//-----------------------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------------------
FileHandler::FileHandler( const std::string& a_fileName
                        , const int a_limit
                        , const int a_count
                        , const bool a_append
                        )
: m_fileName( a_fileName ), m_limit( a_limit ), m_count( a_count ), m_append( a_append ), m_currentFile( 1 )
{
    open();
}

//-----------------------------------------------------------------------------
//  Copy constructor
//-----------------------------------------------------------------------------
FileHandler::FileHandler( const FileHandler& a_other )
: HandlerBase( a_other )
{
    *this = a_other;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
FileHandler::~FileHandler()
{
    close();
}

//-----------------------------------------------------------------------------
//  close
//-----------------------------------------------------------------------------
void FileHandler::close()
{
    if( 0 != m_stream.is_open() )
        m_stream.close();
}

//-----------------------------------------------------------------------------
//  publish
//-----------------------------------------------------------------------------
void FileHandler::publish( const LogRecord& a_record )
{
    // Publish record at all?

    if( false == isLoggable( a_record ) )
        return;

    // Valid stream?

    if( false == m_stream.is_open() )
        return;

    // Write formatted message to file

    m_stream << m_pFormatter->format( a_record ).c_str();
    m_stream.flush();

    // Check file size limit

    if( m_limit > 0 && m_stream.tellp() > m_limit )
    {
        close();
        ++m_currentFile;
        open();
    }
}

//-----------------------------------------------------------------------------
//  clone
//-----------------------------------------------------------------------------
IHandler* FileHandler::clone() const
{
    return new FileHandler( *this );
}

//-----------------------------------------------------------------------------
//  Copy assignment operator
//-----------------------------------------------------------------------------
const FileHandler& FileHandler::operator =( const FileHandler& a_other )
{
    // Close current stream

    close();

    // Copy relevant members

    m_fileName      = a_other.m_fileName;
    m_limit         = a_other.m_limit;
    m_count         = a_other.m_count;
    m_append        = a_other.m_append;
    m_currentFile   = a_other.m_currentFile;

    // Open stream

    open();

    return *this;
}

//*************************************************************************************************
//*************************************************************************************************
//**    Private methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  open
//-----------------------------------------------------------------------------
void FileHandler::open()
{
    // Adjust members

    if( true == m_fileName.empty() )
        m_fileName = "default";

    m_limit = Max( 0, m_limit );
    m_count = Max( 1, m_count );

    if( m_currentFile > m_count )
        m_currentFile = 1;

    // Determine file name to write to
    
    std::ostringstream fileName;

    if( m_count > 1 )
        fileName << m_fileName << "_" << m_currentFile << ".log";
    else
        fileName << m_fileName << ".log";

    // Open file stream

    m_stream.open( fileName.str().c_str(), m_append ? std::ios::app : std::ios::out );
}
