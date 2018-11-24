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

#ifndef         IFILTER_H_INCLUDED
#define         IFILTER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

namespace Logging
{

class           LogRecord;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  IFilter
//-----------------------------------------------------------------------------
//
//! A Filter can be used to provide fine grain control over what is logged,
//! beyond the control provided by log levels
//
//-----------------------------------------------------------------------------
class IFilter
{

public:

    //
    // Queries
    //

    //! \brief  Checks if a given log record should be published
    //! \param  [in] A LogRecord
    //! \return The method returns either of the following values
    //! \retval true    | The log record should be published
    //! \retval false   | The log record should not be published
    virtual const bool  isLoggable  ( const LogRecord& a_record ) const = 0;

    //
    // Operations
    //

    //! \brief  Creates a copy of this Filter
    //! \return A new Filter instance
    virtual IFilter*    clone       () const = 0;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // IFILTER_H_INCLUDED
