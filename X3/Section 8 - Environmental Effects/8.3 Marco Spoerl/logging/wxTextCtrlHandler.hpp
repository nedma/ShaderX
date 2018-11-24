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

#ifndef         WXTEXTCTRLHANDLER_H_INCLUDED
#define         WXTEXTCTRLHANDLER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "HandlerBase.hpp"

class           wxTextCtrl;

namespace Logging
{

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  wxTextCtrlHandler
//-----------------------------------------------------------------------------
//
//! This Handler publishes log records to cerr
//
//-----------------------------------------------------------------------------
class wxTextCtrlHandler : public HandlerBase
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a wxTextCtrlHandler
    //! \param  [in] The text control to append the messages to
                                        wxTextCtrlHandler   ( wxTextCtrl* a_pTextCtrl );
    //! \brief  Creates a wxTextCtrlHandler from the specified one
    //! \param  [in] The wxTextCtrlHandler to copy from
                                        wxTextCtrlHandler   ( const wxTextCtrlHandler& a_other );
    //! \brief  Destructor
    virtual                             ~wxTextCtrlHandler  ();

    //
    // Operations
    //

    //! \brief  Publishs a LogRecord
    //! \param  [in] Description of the log event
    virtual void                        publish             ( const LogRecord& a_record );

    //! \brief  Creates a copy of this Handler
    //! \return A new Handler instance
    virtual IHandler*                   clone               () const;

    //
    // Operators
    //

    //! \brief  Sets this wxTextCtrlHandler to the specified one
    //! \param  [in] The wxTextCtrlHandler to copy from
    //! \return The modified wxTextCtrlHandler
    virtual const wxTextCtrlHandler&    operator =          ( const wxTextCtrlHandler& a_other );

private:

    //
    // Private data
    //

    wxTextCtrl* m_pTextCtrl; //!< The wxWidgets text control to append to

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // WXTEXTCTRLHANDLER_H_INCLUDED
