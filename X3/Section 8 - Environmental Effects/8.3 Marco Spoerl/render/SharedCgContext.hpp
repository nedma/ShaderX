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

#ifndef         SHAREDCGCONTEXT_H_INCLUDED
#define         SHAREDCGCONTEXT_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <Cg/cg.h>

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  SharedCgContext
//-----------------------------------------------------------------------------
//
//! Shared Cg context and profiles
//
//-----------------------------------------------------------------------------
class SharedCgContext
{

public:

    //
    // Create/destroy instance
    //

    //! \brief  Returns the one and only instance of SharedCgContext
    //! \return The instance
    static SharedCgContext& getInstance         ();
    //! \brief  Destroys the one and only instance of SharedCgContext
    static void             destroy             ();

    //
    // Accessors
    //

    //! \brief  Returns the current Cg context
    //! \return The current Cg context
    const CGcontext&        getContext          () const;
    //! \brief  Returns the current Cg vertex profile
    //! \return The current Cg vertex profile
    const CGprofile&        getVertexProfile    () const;
    //! \brief  Returns the current Cg fragment profile
    //! \return The current Cg fragment profile
    const CGprofile&        getFragmentProfile  () const;

private:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new SharedCgContext
                            SharedCgContext ();
    //! \brief  Creates a SharedCgContext from the specified one
    //! \param  [in] The SharedCgContext to copy from
                            SharedCgContext ( const SharedCgContext& );
    //! \brief  Destructor
                            ~SharedCgContext();
    //! \brief  Sets this SharedCgContext to the specified one
    //! \param  [in] The SharedCgContext to copy from
    //! \return The modified SharedCgContext
    SharedCgContext&        operator=       ( const SharedCgContext& );

    //
    // Static instance
    //

    static SharedCgContext* m_pSharedCgContext; //!< The one and only instance of SharedCgContext

    //
    // Cg data
    //

    CGcontext m_context;          //!< Current Cg context
    CGprofile m_vertexProfile;    //!< Current Cg vertex profile
    CGprofile m_fragmentProfile;  //!< Current Cg framgent profile

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // SHAREDCGCONTEXT_H_INCLUDED
