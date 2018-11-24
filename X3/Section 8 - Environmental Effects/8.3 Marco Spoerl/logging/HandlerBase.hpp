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

#ifndef         HANDLERBASE_H_INCLUDED
#define         HANDLERBASE_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "IHandler.hpp"
#include        "Level.hpp"

namespace Logging
{

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  HandlerBase
//-----------------------------------------------------------------------------
//
//! Basic Handler object
//
//-----------------------------------------------------------------------------
class HandlerBase : public IHandler
{

public:

    //
    // Accessors
    //

    //! \brief  Returns the current Filter for this Handler
    //! \return A Filter object (may be null)
    virtual const IFilter*      getFilter       () const;
    //! \brief  Returns the Formatter for this Handler
    //! \return The Formatter
    virtual const IFormatter&   getFormatter    () const;
    //! \brief  Returns the log level specifying which messages will be logged by this Handler
    //! \return The level of messages being logged
    virtual const Level&        getLevel        () const;

    //! \brief  Sets a Filter to control output on this Handler
    //! \param  [in] A Filter object (may be null)
    virtual void                setFilter       ( const IFilter* a_pFilter );
    //! \brief  Sets the Formatter that will be used to format LogRecords for this Handler
    //! \param  [in] The Formatter to use
    virtual void                setFormatter    ( const IFormatter& a_formatter );
    //! \brief  Sets the log level specifying which message levels will be logged by this Handler
    //! \param  [in] The new value for the log level
    virtual void                setLevel        ( const Level& a_level );

    //
    // Queries
    //

    //! \brief  Checks if this Handler would actually log a given LogRecord
    //! \param  [in] A LogRecord 
    //! \return The method returns either of the following values
    //! \retval true    | The log record would be published
    //! \retval false   | The log record would not be published
    virtual const bool          isLoggable      ( const LogRecord& a_record ) const;

    //
    // Operations
    //

    //! \brief  Closes the Handler and frees all associated resources
    virtual void                close           ();

    //! \brief  Publishs a LogRecord
    //! \param  [in] Description of the log event
    virtual void                publish         ( const LogRecord& a_record );

    //! \brief  Creates a copy of this Handler
    //! \return A new Handler instance
    virtual IHandler*           clone           () const;

protected:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a HandlerBase
                                HandlerBase     ();
    //! \brief  Creates a HandlerBase from the specified one
    //! \param  [in] The HandlerBase to copy from
                                HandlerBase     ( const HandlerBase& a_other );
    //! \brief  Destructor
    virtual                     ~HandlerBase    ();

    //
    // Operators
    //

    //! \brief  Sets this HandlerBase to the specified one
    //! \param  [in] The HandlerBase to copy from
    //! \return The modified HandlerBase
    virtual const HandlerBase&  operator =      ( const HandlerBase& a_other );

    //
    // Protected data
    //

    IFilter*    m_pFilter;      //!< A Filter to control output on this Handler (may be null)
    IFormatter* m_pFormatter;   //!< The Formatter to use
    Level       m_level;        //!< The log level to use

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // HANDLERBASE_H_INCLUDED
