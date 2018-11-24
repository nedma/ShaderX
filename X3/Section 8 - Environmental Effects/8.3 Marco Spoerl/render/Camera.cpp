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

#include        "Camera.hpp"
#include        "../math/CQuaternion.h"

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
Camera::Camera()
: m_position( 0.0f, 0.0f, 0.0f )
, m_view( 0.0f, 0.0f, -1.0f )
, m_up( 0.0f, 1.0f, 0.0f )
, m_FoV( g_fFoV )
, m_nearClip( g_fZNear )
, m_farClip( g_fZFar )
, m_viewportWidth( 1 )
, m_viewportHeight( 1 )
{
    updateCenter();
    updateAspect();
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
Camera::~Camera()
{}

//-----------------------------------------------------------------------------
//  setPosition
//-----------------------------------------------------------------------------
void Camera::setPosition( const CVector3& a_position )
{
    m_position = a_position;
    updateCenter();
}

//-----------------------------------------------------------------------------
//  getPosition
//-----------------------------------------------------------------------------
const CVector3& Camera::getPosition() const
{
    return m_position;
}

//-----------------------------------------------------------------------------
//  setView
//-----------------------------------------------------------------------------
void Camera::setView( const CVector3& a_view )
{
    m_view = a_view;
    updateCenter();
}

//-----------------------------------------------------------------------------
//  getView
//-----------------------------------------------------------------------------
const CVector3& Camera::getView() const
{
    return m_view;
}

//-----------------------------------------------------------------------------
//  getCenter
//-----------------------------------------------------------------------------
const CVector3& Camera::getCenter() const
{
    return m_center;
}

//-----------------------------------------------------------------------------
//  setUp
//-----------------------------------------------------------------------------
void Camera::setUp( const CVector3& a_up )
{
    m_up = a_up;
}

//-----------------------------------------------------------------------------
//  getUp
//-----------------------------------------------------------------------------
const CVector3& Camera::getUp() const
{
    return m_up;
}

//-----------------------------------------------------------------------------
//  setFoV
//-----------------------------------------------------------------------------
void Camera::setFoV( const float a_FoV )
{
    m_FoV = a_FoV;
}

//-----------------------------------------------------------------------------
//  getFoV
//-----------------------------------------------------------------------------
const float Camera::getFoV() const
{
    return m_FoV;
}

//-----------------------------------------------------------------------------
//  setNearClip
//-----------------------------------------------------------------------------
void Camera::setNearClip( const float a_nearClip )
{
    m_nearClip = a_nearClip;
}

//-----------------------------------------------------------------------------
//  getNearClip
//-----------------------------------------------------------------------------
const float Camera::getNearClip() const
{
    return m_nearClip;
}

//-----------------------------------------------------------------------------
//  setFarClip
//-----------------------------------------------------------------------------
void Camera::setFarClip( const float a_farClip )
{
    m_farClip = a_farClip;
}

//-----------------------------------------------------------------------------
//  getFarClip
//-----------------------------------------------------------------------------
const float Camera::getFarClip() const
{
    return m_farClip;
}

//-----------------------------------------------------------------------------
//  setViewportWidth
//-----------------------------------------------------------------------------
void Camera::setViewportWidth( const int a_width )
{
    m_viewportWidth = a_width;
    updateAspect();
}

//-----------------------------------------------------------------------------
//  getViewportWidth
//-----------------------------------------------------------------------------
const int Camera::getViewportWidth() const
{
    return m_viewportWidth;
}

//-----------------------------------------------------------------------------
//  setViewportHeight
//-----------------------------------------------------------------------------
void Camera::setViewportHeight( const int a_height )
{
    m_viewportHeight = a_height;
    updateAspect();
}

//-----------------------------------------------------------------------------
//  getViewportHeight
//-----------------------------------------------------------------------------
const int Camera::getViewportHeight() const
{
    return m_viewportHeight;
}

//-----------------------------------------------------------------------------
//  getAspect
//-----------------------------------------------------------------------------
const float Camera::getAspect() const
{
    return m_aspect;
}

//-----------------------------------------------------------------------------
//  Rotate
//-----------------------------------------------------------------------------
void Camera::rotate( const float a_angle, const float a_x, const float a_y, const float a_z )
{
    CQuaternion temp, tempConjugate, view, result;

    temp.AccessX() = a_x * sinf( a_angle * 0.5f );
    temp.AccessY() = a_y * sinf( a_angle * 0.5f );
    temp.AccessZ() = a_z * sinf( a_angle * 0.5f );
    temp.AccessW() = cosf( a_angle * 0.5f );

    view.AccessX() = m_view.GetX();
    view.AccessY() = m_view.GetY();
    view.AccessZ() = m_view.GetZ();
    view.AccessW() = 0;

    tempConjugate.AccessX() = -temp.GetX();
    tempConjugate.AccessY() = -temp.GetY();
    tempConjugate.AccessZ() = -temp.GetZ();

    result = (temp * view) * tempConjugate;

    m_view.AccessX() = result.GetX();
    m_view.AccessY() = result.GetY();
    m_view.AccessZ() = result.GetZ();

    updateCenter();
}

//*************************************************************************************************
//*************************************************************************************************
//**    Private methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  updateCenter
//-----------------------------------------------------------------------------
void Camera::updateCenter()
{
    m_center = m_position + m_view;
}

//-----------------------------------------------------------------------------
//  updateAspect
//-----------------------------------------------------------------------------
void Camera::updateAspect()
{
    m_aspect = static_cast<float>(m_viewportWidth) / static_cast<float>(m_viewportHeight);
}
