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

#ifndef         TEXTRENDERER_H_INCLUDED
#define         TEXTRENDERER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "IRenderer.hpp"
#include        "../glfont/GLFont.hpp"
#include        <string>

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  TextRenderer
//-----------------------------------------------------------------------------
//
//! Displays basic information
//
//-----------------------------------------------------------------------------
class TextRenderer : public IRenderer
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new TextRenderer
    //! \param  [in] The RGB image data to use
    //! \param  [in] The image width
    //! \param  [in] The image height
                    TextRenderer    ( const unsigned char* a_pTexture, const int a_width, const int a_height );
    //! \brief  Destructor
                    ~TextRenderer   ();

    //
    // Interface IRender implementation
    //

    //! \brief  Renders the text
    //! \param  [in] The camera to use
    //! \param  [in] Flag indicating whether to render in wireframe mode
    void            render          ( const Camera& a_camera, const bool a_wireframe );

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a TextRenderer from the specified one
    //! \param  [in] The TextRenderer to copy from
                    TextRenderer    ( const TextRenderer& );
    //! \brief  Sets this TextRenderer to the specified one
    //! \param  [in] The TextRenderer to copy from
    //! \return The modified TextRenderer
    TextRenderer&   operator=       ( const TextRenderer& );

    //
    // Font object
    //

    unsigned int    m_texture;  //!< Font texture
    GLFont          m_font;     //!< Font used for rendering

    //
    // Text to be displayed
    //

    std::string     m_renderer;         //!< Used OpenGL renderer
    std::string     m_version;          //!< OpenGL renderer version
    std::string     m_vertexProfile;    //!< Used Cg vertex profile
    std::string     m_fragmentProfile;  //!< Used Cg fragment profile

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // TEXTRENDERER_H_INCLUDED
