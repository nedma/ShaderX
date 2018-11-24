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

#include        "app/AppFrame.hpp"
#include        "../logging/Logger.hpp"
#include        "../logging/FileHandler.hpp"

using Logging::Logger;

//-----------------------------------------------------------------------------
//  SkyApp
//-----------------------------------------------------------------------------
//
//! The sky application
//
//-----------------------------------------------------------------------------
class SkyApp : public wxApp
{

    virtual bool OnInit();

};

IMPLEMENT_APP( SkyApp )

//-----------------------------------------------------------------------------
//  OnInit
//-----------------------------------------------------------------------------
bool SkyApp::OnInit()
{
    // Initialize logging framework

    Logging::FileHandler fileHandler( "daysky" );

    fileHandler.setLevel( Logging::DEBUG );

    Logger::getInstance().setLevel( Logging::DEBUG );
    Logger::getInstance().addHandler( fileHandler );

    // Create application frame

    AppFrame* pFrame = new AppFrame( "Daysky", wxDefaultPosition, wxSize(800, 450) );

    if( false == pFrame->initOpenGLDeps() )
    {
        delete pFrame;
        return false;
    }

    // Start application

    pFrame->Show( true );
    SetTopWindow( pFrame );

    return true;
}
