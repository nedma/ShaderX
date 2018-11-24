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

#ifndef         LEVEL_H_INCLUDED
#define         LEVEL_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <string>

namespace Logging
{

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Level
//-----------------------------------------------------------------------------
//
//! The Level class defines a set of standard logging levels that can be used
//! to control logging output
//
//-----------------------------------------------------------------------------
class Level
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Creates a named Level with a given integer value
    //! \param  [in] The name of the Level
    //! \param  [in] An integer value for the Level
                        Level       ( const std::string& a_name, const int a_value );
    //! \brief  Creates a named Level from the specified one
    //! \param  [in] The Level to copy from
                        Level       ( const Level& a_other );
    //! \brief  Destructor
                        ~Level      ();

    //
    // Accessors
    //

    //! \brief  Returns the string name of the Level
    //! \return The name of the Level
    const std::string&  getName     () const;
    //! \brief  Returns the integer value for this Level
    //! \return The integer value for this Level
    const int           getValue    () const;

    //
    // Operators
    //

    //! \brief  Sets this Level to the specified one
    //! \param  [in] The Level to copy from
    //! \return The modified Level
    const Level&        operator =  ( const Level& a_other );

private:

    //
    // Private data
    //

    std::string m_name;     //!< The name of the Level
    int         m_value;    //!< The integer value for this Level

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

const Level DEBUG ( "DEBUG", 0 );   //!< Pre-defined Level for debugging messages
const Level INFO  ( "INFO" , 1 );   //!< Pre-defined Level for information messages
const Level WARN  ( "WARN" , 2 );   //!< Pre-defined Level for warning messages
const Level ERR   ( "ERROR", 3 );   //!< Pre-defined Level for error messages
const Level FATAL ( "FATAL", 4 );   //!< Pre-defined Level for fatal error messages

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // LOGGER_H_INCLUDED
