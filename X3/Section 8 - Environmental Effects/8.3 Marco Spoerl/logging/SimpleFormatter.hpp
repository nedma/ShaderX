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

#ifndef         SIMPLEFORMATTER_H_INCLUDED
#define         SIMPLEFORMATTER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "IFormatter.hpp"

namespace Logging
{

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  SimpleFormatter
//-----------------------------------------------------------------------------
//
//! Prints a brief summary of the LogRecord in a human readable format
//
//-----------------------------------------------------------------------------
class SimpleFormatter : public IFormatter
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a SimpleFormatter
                                    SimpleFormatter     ();
    //! \brief  Creates a SimpleFormatter from the specified one
    //! \param  [in] The SimpleFormatter to copy from
                                    SimpleFormatter     ( const SimpleFormatter& a_other );
    //! \brief  Destructor
    virtual                         ~SimpleFormatter    ();

    //
    // Operations
    //

    //! \brief  Formats the given log record and returns the formatted string
    //! \param  [in] The log record to be formatted
    //! \return The formatted log record
    virtual const std::string       format              ( const LogRecord& a_record );

    //! \brief  Creates a copy of this Formatter
    //! \return A new Formatter instance
    virtual IFormatter*             clone               () const;

    //
    // Operators
    //

    //! \brief  Sets this LogRecord to the specified one
    //! \param  [in] The LogRecord to copy from
    //! \return The modified LogRecord
    virtual const SimpleFormatter&  operator =          ( const SimpleFormatter& a_other );

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // SIMPLEFORMATTER_H_INCLUDED
