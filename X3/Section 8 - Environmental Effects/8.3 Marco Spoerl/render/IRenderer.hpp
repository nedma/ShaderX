//*****************************************************************************
//
//  Copyright (c) 2003 Marco Spoerl
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

#ifndef         IRENDERER_H_INCLUDED
#define         IRENDERER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

class           Camera;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  IRenderer
//-----------------------------------------------------------------------------
//
//! Interface for render classes
//
//-----------------------------------------------------------------------------
class IRenderer
{

public:

    //! \brief  Renders the object's content
    //! \param  [in] The camera to use
    //! \param  [in] Flag indicating whether to render in wireframe mode
    virtual void    render  ( const Camera& a_camera, const bool a_wireframe ) = 0;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // IRENDERER_H_INCLUDED
