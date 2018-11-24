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

#ifndef         CAMERA_H_INCLUDED
#define         CAMERA_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "../math/CVector3.h"

class           CAxisAngle;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Camera
//-----------------------------------------------------------------------------
//
//! Very basic quaternion based camera
//
//-----------------------------------------------------------------------------
class Camera
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new camera
                    Camera              ();
    //! \brief  Destructor
                    ~Camera             ();

    //
    // Accessors
    //

    //! \brief  Sets the camera position
    //! \param  [in] The camera position to set
    void            setPosition         ( const CVector3& a_position );
    //! \brief  Returns the camera position
    //! \return The camera position
    const CVector3& getPosition         () const;

    //! \brief  Sets the view vector
    //! \param  [in] The view vector to set
    void            setView             ( const CVector3& a_view );
    //! \brief  Returns the view vector
    //! \return The view vector
    const CVector3& getView             () const;

    //! \brief  Returns the center vector
    //! \return The center vector
    const CVector3& getCenter           () const;

    //! \brief  Sets the up vector
    //! \param  [in] The up vector to set
    void            setUp               ( const CVector3& a_up );
    //! \brief  Returns the up vector
    //! \return The up vector
    const CVector3& getUp               () const;

    //! \brief  Sets the field of view
    //! \param  [in] The field of view to set
    void            setFoV              ( const float a_FoV );
    //! \brief  Returns the field of view
    //! \return The field of view
    const float     getFoV              () const;

    //! \brief  Sets the near clipping distance
    //! \param  [in] The near clipping distance to set
    void            setNearClip         ( const float a_nearClip );
    //! \brief  Returns the near clipping distance
    //! \return The near clipping distance
    const float     getNearClip         () const;

    //! \brief  Sets the far clipping distance
    //! \param  [in] The far clipping distance to set
    void            setFarClip          ( const float a_farClip );
    //! \brief  Returns the far clipping distance
    //! \return The far clipping distance
    const float     getFarClip          () const;

    //! \brief  Sets the viewport width
    //! \param  [in] The viewport width to set
    void            setViewportWidth    ( const int a_width );
    //! \brief  Returns the viewport width
    //! \return The viewport width
    const int       getViewportWidth    () const;

    //! \brief  Sets the viewport height
    //! \param  [in] The viewport height to set
    void            setViewportHeight   ( const int a_height );
    //! \brief  Returns the viewport height
    //! \return The viewport height
    const int       getViewportHeight   () const;

    //! \brief  Returns the c
    //! \return The viewport height
    const float     getAspect           () const;

    //
    // Manipulation
    //

    //! \brief  Rotates the camera
    //! \param  [in] The rotation angle in radians
    //! \param  [in] The rotation's axis' x-component
    //! \param  [in] The rotation's axis' y-component
    //! \param  [in] The rotation's axis' z-component
    void            rotate              ( const float a_angle, const float a_x, const float a_y, const float a_z );

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a Camera from the specified one
    //! \param  [in] The Camera to copy from
                    Camera              ( const Camera& );
    //! \brief  Sets this Camera to the specified one
    //! \param  [in] The Camera to copy from
    //! \return The modified Camera
    Camera&         operator=           ( const Camera& );

    //
    // Update data dependencies
    //

    //! \brief  Updates the camera center vector
    void            updateCenter        ();
    //! \brief  Updates the camera aspect
    void            updateAspect        ();

    //
    // Camera vectors
    //

    CVector3    m_position; //!< Camera position
    CVector3    m_view;     //!< View direction
    CVector3    m_center;   //!< Center vector
    CVector3    m_up;       //!< Up vector

    //
    // Clipping data
    //

    float       m_FoV;      //!< Field of view
    float       m_nearClip; //!< Near clipping distance
    float       m_farClip;  //!< Far clipping distance

    //
    // Viewport data
    //

    int         m_viewportWidth;    //!< Viewport width
    int         m_viewportHeight;   //!< Viewport height
    float       m_aspect;           //!< Aspect ratio

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // CAMERA_H_INCLUDED
