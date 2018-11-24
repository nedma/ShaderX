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

#ifndef         LOGGER_H_INCLUDED
#define         LOGGER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "Level.hpp"
#include        <vector>

namespace Logging
{

class           IHandler;
class           IFilter;
class           LogRecord;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Logger
//-----------------------------------------------------------------------------
//
//! A Logger object is used to log messages
//
//-----------------------------------------------------------------------------
class Logger
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Returns the one and only instance of Logger
    //! \return The one and only instance of Logger
    static Logger&  getInstance     ();
    //! \brief  Destroys the one and only instance of Logger
    static void     destroy         ();

    //
    // Modification / Accessors / Queries
    //

    //! \brief  Adds a log Handler to receive logging messages
    //! \param  [in] A logging Handler 
    void            addHandler      ( const IHandler& a_handler );

    //! \brief  Returns the current filter for this Logger
    //! \return A filter object (may be null)
    const IFilter*  getFilter       () const;
    //! \brief  Returns the log level that has been specified for this Logger
    //! \return This Logger's level
    const Level&    getLevel        () const;

    //! \brief  Sets a filter to control output on this Logger
    //! \param  [in] A filter object (may be null)
    void            setFilter       ( const IFilter* a_pFilter );
    //! \brief  Sets the log level specifying which message levels will be logged by this Logger
    //! \param  [in] The new value for the log level 
    void            setLevel        ( const Level& a_level );

    //! \brief  Checks if a message of the given level would actually be logged by this logger
    //! \param  [in] A message logging level 
    //! \return The method returns either of the following values
    //! \retval true    | The message would be logged
    //! \retval false   | The message would not be logged
    const bool      isLoggable      ( const Level& a_level ) const;

    //
    // General logging methods
    //

    //! \brief  Logs a message
    //! \param  [in] One of the message level identifiers
    //! \param  [in] The string message
    void            log             ( const Level& a_level
                                    , const std::string& a_message
                                    );
    //! \brief  Logs a LogRecord
    //! \param  [in] The LogRecord to be published
    void            log             ( const LogRecord& a_record );
    //! \brief  Logs a message, specifying source class and method
    //! \param  [in] One of the message level identifiers
    //! \param  [in] Name of class that issued the logging request
    //! \param  [in] Name of method that issued the logging request
    //! \param  [in] The string message 
    void            log             ( const Level& a_level
                                    , const std::string& a_sourceClass
                                    , const std::string& a_sourceMethod
                                    , const std::string& a_message
                                    );
                                    
    //
    // Convenience logging methods
    //

    //! \brief  Log a method entry
    //! \param  [in] Name of class that issued the logging request
    //! \param  [in] Name of method that is being entered
    void            entering        ( const std::string& a_sourceClass
                                    , const std::string& a_sourceMethod
                                    );
    //! \brief  Log a method return
    //! \param  [in] Name of class that issued the logging request
    //! \param  [in] Name of the method
    void            exiting         ( const std::string& a_sourceClass
                                    , const std::string& a_sourceMethod
                                    );
    //! \brief  Log a debug message
    //! \param  [in] The string message 
    void            logDebug        ( const std::string& a_message );
    //! \brief  Log a debug message, specifying source class and method
    //! \param  [in] Name of class that issued the logging request
    //! \param  [in] Name of method that is being entered
    //! \param  [in] The string message 
    void            logDebug        ( const std::string& a_sourceClass
                                    , const std::string& a_sourceMethod
                                    , const std::string& a_message
                                    );
    //! \brief  Log a information message
    //! \param  [in] The string message 
    void            logInfo         ( const std::string& a_message );
    //! \brief  Log a information message, specifying source class and method
    //! \param  [in] Name of class that issued the logging request
    //! \param  [in] Name of method that is being entered
    //! \param  [in] The string message 
    void            logInfo         ( const std::string& a_sourceClass
                                    , const std::string& a_sourceMethod
                                    , const std::string& a_message
                                    );
    //! \brief  Log a warning message
    //! \param  [in] The string message 
    void            logWarning      ( const std::string& a_message );
    //! \brief  Log a warning message, specifying source class and method
    //! \param  [in] Name of class that issued the logging request
    //! \param  [in] Name of method that is being entered
    //! \param  [in] The string message 
    void            logWarning      ( const std::string& a_sourceClass
                                    , const std::string& a_sourceMethod
                                    , const std::string& a_message
                                    );
    //! \brief  Log an error message
    //! \param  [in] The string message 
    void            logError        ( const std::string& a_message );
    //! \brief  Log an error message, specifying source class and method
    //! \param  [in] Name of class that issued the logging request
    //! \param  [in] Name of method that is being entered
    //! \param  [in] The string message 
    void            logError        ( const std::string& a_sourceClass
                                    , const std::string& a_sourceMethod
                                    , const std::string& a_message
                                    );
    //! \brief  Log a fatal error message
    //! \param  [in] The string message 
    void            logFatal        ( const std::string& a_message );
    //! \brief  Log a fatal error message, specifying source class and method
    //! \param  [in] Name of class that issued the logging request
    //! \param  [in] Name of method that is being entered
    //! \param  [in] The string message 
    void            logFatal        ( const std::string& a_sourceClass
                                    , const std::string& a_sourceMethod
                                    , const std::string& a_message
                                    );

private:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a Logger
                    Logger      ();
    //! \brief  Creates a Logger from the specified one
    //! \param  [in] The Logger to copy from
                    Logger      ( const Logger& a_other );
    //! \brief  Destructor
                    ~Logger     ();

    //
    // Operators
    //

    //! \brief  Sets this Logger to the specified one
    //! \param  [in] The Logger to copy from
    //! \return The modified Logger
    Logger&         operator=   ( const Logger& a_other );

    //
    // Operations
    //

    //! \brief  Publishs a LogRecord
    //! \param  [in] Description of the log event
    void            publish     ( const LogRecord& a_record );

    //
    // Private data
    //

    IFilter*                    m_pFilter;  //!< A Filter to control output on this Logger (may be null)
    Level                       m_level;    //!< The log level to use
    std::vector< IHandler* >    m_handlers; //!< All handlers receiving messages from this Logger

    static Logger*              m_pLogger;  //!< The one and only instance of Logger

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // LOGGER_H_INCLUDED
