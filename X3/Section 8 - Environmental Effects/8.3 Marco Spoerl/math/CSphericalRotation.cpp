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

#include        "CSphericalRotation.h"

//*********************************************************************************************************************
//*********************************************************************************************************************
//  Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//*****************************************************************************
//  Constructor
//*****************************************************************************
//
//! \brief      Copy constructor
//! \param      [in] A reference to a constant class to copy from
//
//*****************************************************************************
CSphericalRotation::CSphericalRotation( const CSphericalRotation& a_Other )
{
    *this = a_Other;
}

//*****************************************************************************
//  Constructor
//*****************************************************************************
//
//! \brief      Default constructor
//
//*****************************************************************************
CSphericalRotation::CSphericalRotation()
{
}

//*****************************************************************************
//  Destructor
//*****************************************************************************
//
//! \brief      Destructor
//
//*****************************************************************************
CSphericalRotation::~CSphericalRotation()
{
}

//*****************************************************************************
//  operator =
//*****************************************************************************
//
//! \brief      Makes this class a copy of the specified class
//! \param      [in] A reference to a constant class to copy from
//! \return     Returns a reference to self
//
//*****************************************************************************
CSphericalRotation& CSphericalRotation::operator = ( const CSphericalRotation& a_Other )
{
    m_fLatitude     = a_Other.m_fLatitude;
    m_fLongitude    = a_Other.m_fLongitude;
    m_fAzimuth      = a_Other.m_fAzimuth;

    return *this;
}
