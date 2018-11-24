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

#ifndef         LOGRECORD_H_INCLUDED
#define         LOGRECORD_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "Level.hpp"
#include        <string>

namespace Logging
{

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  LogRecord
//-----------------------------------------------------------------------------
//
//! LogRecord objects are used to pass logging requests between the logging
//! framework and individual log Handlers
//
//-----------------------------------------------------------------------------
class LogRecord
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a LogRecord with the given level and message values
    //! \param  [in] A logging level value
    //! \param  [in] The raw logging message
                        LogRecord           ( const Level& a_level, const std::string& a_message );
    //! \brief  Creates a LogRecord from the specified one
    //! \param  [in] The LogRecord to copy from
                        LogRecord           ( const LogRecord& a_other );
    //! \brief  Destructor
                        ~LogRecord          ();

    //
    // Accessors
    //

    //! \brief  Returns the logging message level
    //! \return The logging message level
    const Level&        getLevel            () const;
    //! \brief  Returns the raw log message, before formatting
    //! \return The raw message string
    const std::string&  getMessage          () const;
    //! \brief  Returns the event time in seconds since midnight, January 1, 1970
    //! \return The event time in seconds since midnight, January 1, 1970
    const time_t        getTime             () const;
    //! \brief  Returns the sequence number
    //! \return The sequence number
    const long          getSequenceNumber   () const;

    //! \brief  Sets the logging message level
    //! \param  [in] The logging message level
    void                setLevel            ( const Level& a_level );
    //! \brief  Sets the raw log message, before formatting
    //! \param  [in] The raw message string
    void                setMessage          ( const std::string& a_message );
    //! \brief  Sets the event time
    //! \param  [in] The event time in seconds since midnight, January 1, 1970
    void                setTime             ( const time_t a_time );
    //! \brief  Sets the sequence number
    //! \param  [in] The sequence number to set
    void                setSequenceNumber   ( const long a_sequenceNumber );

    //
    // Operators
    //

    //! \brief  Sets this LogRecord to the specified one
    //! \param  [in] The LogRecord to copy from
    //! \return The modified LogRecord
    const LogRecord&    operator =          ( const LogRecord& a_other );

private:

    //
    // Private data
    //

    Level       m_level;                //!< The logging message level
    std::string m_message;              //!< The raw message string
    time_t      m_time;                 //!< The event time in seconds since midnight, January 1, 1970
    long        m_sequenceNumber;       //!< The sequence number

    static long s_nextSequenceNumber;   //!< The next sequence number to use

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // LOGRECORD_H_INCLUDED
