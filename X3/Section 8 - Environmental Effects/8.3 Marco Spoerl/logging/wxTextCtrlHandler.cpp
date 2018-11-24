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

#include        "wxTextCtrlHandler.hpp"
#include        "IFormatter.hpp"
#include        "LogRecord.hpp"
#include        <wx/textctrl.h>

using namespace Logging;

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
wxTextCtrlHandler::wxTextCtrlHandler( wxTextCtrl* a_pTextCtrl )
: m_pTextCtrl( a_pTextCtrl )
{
}

//-----------------------------------------------------------------------------
//  Copy constructor
//-----------------------------------------------------------------------------
wxTextCtrlHandler::wxTextCtrlHandler( const wxTextCtrlHandler& a_other )
: HandlerBase( a_other )
{
    *this = a_other;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
wxTextCtrlHandler::~wxTextCtrlHandler()
{
}

//-----------------------------------------------------------------------------
//  publish
//-----------------------------------------------------------------------------
void wxTextCtrlHandler::publish( const LogRecord& a_record )
{
    // Publish record at all?

    if( false == isLoggable( a_record ) )
        return;

    if( NULL == m_pTextCtrl )
        return;

    // Set text color

    switch( a_record.getLevel().getValue() )
    {

    // DEBUG

    case 0:
        m_pTextCtrl->SetDefaultStyle( wxTextAttr( *wxBLUE ) );
        break;

    // INFO

    case 1:
        m_pTextCtrl->SetDefaultStyle( wxTextAttr( *wxBLACK ) );
        break;

    // WARN

    case 2:
        m_pTextCtrl->SetDefaultStyle( wxTextAttr( *wxGREEN ) );
        break;

    // ERR

    case 3:
        m_pTextCtrl->SetDefaultStyle( wxTextAttr( *wxRED ) );
        break;

    // FATAL

    case 4:
        m_pTextCtrl->SetDefaultStyle( wxTextAttr( *wxRED ) );
        break;

    default:
        m_pTextCtrl->SetDefaultStyle( wxTextAttr( *wxLIGHT_GREY ) );
        break;

    }

    // Write formatted message to text control

    m_pTextCtrl->AppendText( m_pFormatter->format( a_record ).c_str() );
}

//-----------------------------------------------------------------------------
//  clone
//-----------------------------------------------------------------------------
IHandler* wxTextCtrlHandler::clone() const
{
    return new wxTextCtrlHandler( *this );
}

//-----------------------------------------------------------------------------
//  Copy assignment operator
//-----------------------------------------------------------------------------
const wxTextCtrlHandler& wxTextCtrlHandler::operator =( const wxTextCtrlHandler& a_other )
{
    m_pTextCtrl = a_other.m_pTextCtrl;

    return *this;
}
