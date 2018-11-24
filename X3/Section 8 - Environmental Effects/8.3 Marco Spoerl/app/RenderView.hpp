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

#ifndef         RENDERVIEW_H_INCLUDED
#define         RENDERVIEW_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <wx/glcanvas.h>
#include        "../render/Camera.hpp"
#include        "../render/IRenderer.hpp"
#include        <vector>

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  RenderView
//-----------------------------------------------------------------------------
//
//! The sky application render view
//
//-----------------------------------------------------------------------------
class RenderView : public wxGLCanvas
{

public:

    //
    // Con-/Destruction
    //

                RenderView          ( wxWindow* a_pParent );
                ~RenderView         ();

    //
    // Event callbacks
    //

    void        OnClick             ( wxMouseEvent& a_event );
    void        OnEraseBackground   ( wxEraseEvent& a_event );
    void        OnMotion            ( wxMouseEvent& a_event );
    void        OnPaint             ( wxPaintEvent& a_event );
    void        OnSize              ( wxSizeEvent& a_event );

    //
    // Manipulation
    //

    void        AddRenderer         ( IRenderer* a_pRenderer );
    void        SetWireframe        ( const bool a_wireframe );

    DECLARE_EVENT_TABLE()

private:

    //
    // Construction and assignment
    //

                RenderView          ( const RenderView& );
    RenderView& operator=           ( const RenderView& );

    //
    // Canvas attributes
    //

    static int ms_iAttributes[];

    //
    // Renderer data
    //

    Camera                  m_camera;
    bool                    m_wireframe;
    std::vector<IRenderer*> m_renderer;

    //
    // Mouse capture and camera
    //

    long    m_lastX;
    long    m_lastY;

    float   m_mouseSensitivity;
    float   m_maxAngle;
    float   m_currentRotationAboutX;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // RENDERVIEW_H_INCLUDED
