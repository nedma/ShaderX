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

#ifndef         SKYRENDERER_H_INCLUDED
#define         SKYRENDERER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "IRenderer.hpp"
#include        <Cg/cg.h>

class           SkyDome;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  SkyRenderer
//-----------------------------------------------------------------------------
//
//! Sky dome renderer
//
//-----------------------------------------------------------------------------
class SkyRenderer : public IRenderer
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new SkyRenderer
    //! \param  [in] The sky dome to render
                    SkyRenderer ( const SkyDome* a_pSkyDome );
    //! \brief  Destructor
                    ~SkyRenderer();

    //
    // Interface IRender implementation
    //

    //! \brief  Renders the sky
    //! \param  [in] The camera to use
    //! \param  [in] Flag indicating whether to render in wireframe mode
    void            render      ( const Camera& a_camera, const bool a_wireframe );

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a SkyRenderer from the specified one
    //! \param  [in] The SkyRenderer to copy from
                    SkyRenderer ( const SkyRenderer& );
    //! \brief  Sets this SkyRenderer to the specified one
    //! \param  [in] The SkyRenderer to copy from
    //! \return The modified SkyRenderer
    SkyRenderer&    operator=   ( const SkyRenderer& );

    //
    // SkyRenderer data
    //

    const SkyDome*  m_pSkyDome;                 //!< The sky to render

    static char     s_vertexShaderSource[];     //!< Basic sky vertex shader source
    static char     s_fragmentShaderSource[];   //!< Basic sky fragment shader source

    CGprogram       m_cgVertexShader;           //!< Basic sky vertex shader
    CGprogram       m_cgFragmentShader;         //!< Basic sky fragment shader
    CGparameter     m_cgPosition;               //!< Sky vertex shader position parameter

    bool            m_shaderValid;              //!< Flag indicating whether the default vertex and fragment programs are valid

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // SKYRENDERER_H_INCLUDED
