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

#ifndef         COMPASSRENDERER_H_INCLUDED
#define         COMPASSRENDERER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "IRenderer.hpp"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  CompassRenderer
//-----------------------------------------------------------------------------
//
//! Draws a compass rose
//
//-----------------------------------------------------------------------------
class CompassRenderer : public IRenderer
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new CompassRenderer
    //! \param  [in] The RGB image data to use
    //! \param  [in] The image width
    //! \param  [in] The image height
                        CompassRenderer ( const unsigned char* a_pTexture, const int a_width, const int a_height );
    //! \brief  Destructor
                        ~CompassRenderer();

    //
    // Interface IRender implementation
    //

    //! \brief  Renders the compass
    //! \param  [in] The camera to use
    //! \param  [in] Flag indicating whether to render in wireframe mode
    void                render          ( const Camera& a_camera, const bool a_wireframe );

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a CompassRenderer from the specified one
    //! \param  [in] The CompassRenderer to copy from
                        CompassRenderer ( const CompassRenderer& );
    //! \brief  Sets this CompassRenderer to the specified one
    //! \param  [in] The CompassRenderer to copy from
    //! \return The modified CompassRenderer
    CompassRenderer&    operator=       ( const CompassRenderer& );

    //
    // Compass data
    //

    unsigned int m_texture; //!< The compass texture

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // COMPASSRENDERER_H_INCLUDED
