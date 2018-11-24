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

#include        "SharedCgContext.hpp"
#include        <Cg/cgGL.h>
#include        "../logging/logger.hpp"

using           Logging::Logger;

//*********************************************************************************************************************
//*********************************************************************************************************************
//  Static initialization
//*********************************************************************************************************************
//*********************************************************************************************************************

SharedCgContext* SharedCgContext::m_pSharedCgContext = NULL;

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Local methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  CgErrorCallback
//-----------------------------------------------------------------------------
static void CgErrorCallback()
{
    CGerror error = cgGetError();

    Logger::getInstance().logError( cgGetErrorString( error ) );

    if( CG_COMPILER_ERROR == error )
        Logger::getInstance().logDebug( cgGetLastListing( SharedCgContext::getInstance().getContext() ) );
}

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  getInstance
//-----------------------------------------------------------------------------
SharedCgContext& SharedCgContext::getInstance()
{
    if( NULL == m_pSharedCgContext )
    {
        m_pSharedCgContext = new SharedCgContext;
        //  cgSetErrorCallback( CgErrorCallback );
    }

    return *m_pSharedCgContext;
}

//-----------------------------------------------------------------------------
//  destroy
//-----------------------------------------------------------------------------
void SharedCgContext::destroy()
{
    if( NULL == m_pSharedCgContext )
        return;

    delete m_pSharedCgContext;
    m_pSharedCgContext = NULL;
}

//-----------------------------------------------------------------------------
//  getContext
//-----------------------------------------------------------------------------
const CGcontext& SharedCgContext::getContext() const
{
    return m_context;
}

//-----------------------------------------------------------------------------
//  getVertexProfile
//-----------------------------------------------------------------------------
const CGprofile& SharedCgContext::getVertexProfile() const
{
    return m_vertexProfile;
}

//-----------------------------------------------------------------------------
//  getFragmentProfile
//-----------------------------------------------------------------------------
const CGprofile& SharedCgContext::getFragmentProfile() const
{
    return m_fragmentProfile;
}

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Private methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
SharedCgContext::SharedCgContext()
{
    m_context = cgCreateContext();

    m_vertexProfile = CG_PROFILE_ARBVP1;
    m_fragmentProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT );

    if( CG_PROFILE_UNKNOWN != m_vertexProfile )
        cgGLSetOptimalOptions( m_vertexProfile );

    if( CG_PROFILE_UNKNOWN != m_fragmentProfile )
        cgGLSetOptimalOptions( m_fragmentProfile );
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
SharedCgContext::~SharedCgContext()
{
    cgDestroyContext( m_context );
}
