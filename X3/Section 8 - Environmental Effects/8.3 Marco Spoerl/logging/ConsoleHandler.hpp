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

#ifndef         CONSOLEHANDLER_H_INCLUDED
#define         CONSOLEHANDLER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "HandlerBase.hpp"

namespace Logging
{

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  ConsoleHandler
//-----------------------------------------------------------------------------
//
//! This Handler publishes log records to cerr
//
//-----------------------------------------------------------------------------
class ConsoleHandler : public HandlerBase
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a default ConsoleHandler
                                    ConsoleHandler  ();
    //! \brief  Creates a ConsoleHandler from the specified one
    //! \param  [in] The ConsoleHandler to copy from
                                    ConsoleHandler  ( const ConsoleHandler& a_other );
    //! \brief  Destructor
    virtual                         ~ConsoleHandler ();

    //
    // Operations
    //

    //! \brief  Publishs a LogRecord
    //! \param  [in] Description of the log event
    virtual void                    publish         ( const LogRecord& a_record );

    //! \brief  Creates a copy of this Handler
    //! \return A new Handler instance
    virtual IHandler*               clone           () const;

    //
    // Operators
    //

    //! \brief  Sets this ConsoleHandler to the specified one
    //! \param  [in] The ConsoleHandler to copy from
    //! \return The modified ConsoleHandler
    virtual const ConsoleHandler&   operator =      ( const ConsoleHandler& a_other );

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // CONSOLEHANDLER_H_INCLUDED
