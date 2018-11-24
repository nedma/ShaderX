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

#ifndef         IFORMATTER_H_INCLUDED
#define         IFORMATTER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <string>

namespace Logging
{

class           LogRecord;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  IFormatter
//-----------------------------------------------------------------------------
//
//! A Formatter provides support for formatting LogRecords
//
//-----------------------------------------------------------------------------
class IFormatter
{

public:

    //
    // Operations
    //

    //! \brief  Formats the given log record and returns the formatted string
    //! \param  [in] The log record to be formatted
    //! \return The formatted log record
    virtual const std::string   format  ( const LogRecord& a_record ) = 0;

    //! \brief  Creates a copy of this Formatter
    //! \return A new Formatter instance
    virtual IFormatter*         clone   () const = 0;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // IFORMATTER_H_INCLUDED
