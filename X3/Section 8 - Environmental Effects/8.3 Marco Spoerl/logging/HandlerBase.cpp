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

#include        "HandlerBase.hpp"
#include        "IFilter.hpp"
#include        "LogRecord.hpp"
#include        "SimpleFormatter.hpp"

using namespace Logging;

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  getFilter
//-----------------------------------------------------------------------------
const IFilter* HandlerBase::getFilter() const
{
    return m_pFilter;
}

//-----------------------------------------------------------------------------
//  getFormatter
//-----------------------------------------------------------------------------
const IFormatter& HandlerBase::getFormatter() const
{
    return *m_pFormatter;
}

//-----------------------------------------------------------------------------
//  getLevel
//-----------------------------------------------------------------------------
const Level& HandlerBase::getLevel() const
{
    return m_level;
}

//-----------------------------------------------------------------------------
//  setFilter
//-----------------------------------------------------------------------------
void HandlerBase::setFilter( const IFilter* a_pFilter )
{
    // Delete current filter instance

    delete m_pFilter;

    // Special case when argument is NULL

    if( NULL == a_pFilter )
    {
        m_pFilter = NULL;
        return;
    }

    // Create new filter instance

    m_pFilter = a_pFilter->clone();
}

//-----------------------------------------------------------------------------
//  setFormatter
//-----------------------------------------------------------------------------
void HandlerBase::setFormatter( const IFormatter& a_formatter )
{
    // Delete current formatter instance

    delete m_pFormatter;

    // Create new formatter instance

    m_pFormatter = a_formatter.clone();
}

//-----------------------------------------------------------------------------
//  setLevel
//-----------------------------------------------------------------------------
void HandlerBase::setLevel( const Level& a_level )
{
    m_level = a_level;
}

//-----------------------------------------------------------------------------
//  isLoggable
//-----------------------------------------------------------------------------
const bool HandlerBase::isLoggable( const LogRecord& a_record ) const
{
    // Check level value
    // Lower priorities are ignored

    if( a_record.getLevel().getValue() < m_level.getValue() )
        return false;

    // Check filter, if any

    if ( NULL != m_pFilter && false == m_pFilter->isLoggable( a_record ) )
        return false;

    return true;
}

//-----------------------------------------------------------------------------
//  close
//-----------------------------------------------------------------------------
void HandlerBase::close()
{
}

//-----------------------------------------------------------------------------
//  publish
//-----------------------------------------------------------------------------
void HandlerBase::publish( const LogRecord& a_record )
{
}

//-----------------------------------------------------------------------------
//  clone
//-----------------------------------------------------------------------------
IHandler* HandlerBase::clone() const
{
    return NULL;
}

//*************************************************************************************************
//*************************************************************************************************
//**    Protected methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
HandlerBase::HandlerBase()
: m_pFilter( NULL )
, m_pFormatter( new SimpleFormatter )
, m_level( INFO )
{
}

//-----------------------------------------------------------------------------
//  Copy constructor
//-----------------------------------------------------------------------------
HandlerBase::HandlerBase( const HandlerBase& a_other )
: m_pFilter( NULL )
, m_pFormatter( NULL )
, m_level( a_other.getLevel() )
{
    *this = a_other;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
HandlerBase::~HandlerBase()
{
    delete m_pFilter;
    delete m_pFormatter;
}

//-----------------------------------------------------------------------------
//  Copy assignment operator
//-----------------------------------------------------------------------------
const HandlerBase& HandlerBase::operator =( const HandlerBase& a_other )
{
    setFilter   ( a_other.getFilter() );
    setFormatter( a_other.getFormatter() );
    setLevel    ( a_other.getLevel() );

    return *this;
}
