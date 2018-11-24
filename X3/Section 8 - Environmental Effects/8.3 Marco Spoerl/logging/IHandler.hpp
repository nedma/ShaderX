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

#ifndef         IHANDLER_H_INCLUDED
#define         IHANDLER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

namespace Logging
{

class           IFilter;
class           IFormatter;
class           Level;
class           LogRecord;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  IHandler
//-----------------------------------------------------------------------------
//
//! A Handler object takes log messages from a Logger and exports them
//
//-----------------------------------------------------------------------------
class IHandler
{

public:

    //
    // Accessors
    //

    //! \brief  Returns the current Filter for this Handler
    //! \return A Filter object (may be null)
    virtual const IFilter*      getFilter       () const = 0;
    //! \brief  Returns the Formatter for this Handler
    //! \return The Formatter
    virtual const IFormatter&   getFormatter    () const = 0;
    //! \brief  Returns the log level specifying which messages will be logged by this Handler
    //! \return The level of messages being logged
    virtual const Level&        getLevel        () const = 0;

    //! \brief  Sets a Filter to control output on this Handler
    //! \param  [in] A Filter object (may be null)
    virtual void                setFilter       ( const IFilter* a_pFilter ) = 0;
    //! \brief  Sets the Formatter that will be used to format LogRecords for this Handler
    //! \param  [in] The Formatter to use
    virtual void                setFormatter    ( const IFormatter& a_formatter ) = 0;
    //! \brief  Sets the log level specifying which message levels will be logged by this Handler
    //! \param  [in] The new value for the log level
    virtual void                setLevel        ( const Level& a_level ) = 0;

    //
    // Queries
    //

    //! \brief  Checks if this Handler would actually log a given LogRecord
    //! \param  [in] A LogRecord 
    //! \return The method returns either of the following values
    //! \retval true    | The log record would be published
    //! \retval false   | The log record would not be published
    virtual const bool          isLoggable      ( const LogRecord& a_record ) const = 0;

    //
    // Operations
    //

    //! \brief  Closes the Handler and frees all associated resources
    virtual void                close           () = 0;

    //! \brief  Publishs a LogRecord
    //! \param  [in] Description of the log event
    virtual void                publish         ( const LogRecord& a_record ) = 0;

    //! \brief  Creates a copy of this Handler
    //! \return A new Handler instance
    virtual IHandler*           clone           () const = 0;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // IHANDLER_H_INCLUDED
