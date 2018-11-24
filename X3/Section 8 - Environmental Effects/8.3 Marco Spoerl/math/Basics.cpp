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

#include        "Basics.h"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

const double    g_dPI           =   3.14159265358979323846;
const double    g_d2PI          =   g_dPI * 2.0;
const double    g_dPIDiv2       =   g_dPI / 2.0;
const double    g_dPIDiv4       =   g_dPI / 4.0;
const double    g_dInvPI        =   1.0 / g_dPI;
const double    g_dE            =   2.71828182845904523536;
const double    g_dDegToRad     =   g_dPI / 180.0;
const double    g_dRadToDeg     =   180.0 / g_dPI;
const double    g_dHuge         =   1.0e+38;
const double    g_dEpsilon      =   1.0e-5;
const double    g_dInvEpsilon   =   1.0e+5;
const double    g_dPosInfinity  =   3.402823466e+38;        
const double    g_dNegInfinity  =  -g_fPosInfinity;
const double    g_dFoV          =   60.0f;
const double    g_dZNear        =   1.0;
const double    g_dZFar         =   1000.0; 

const float     g_fPI           = static_cast<float>( g_dPI          );
const float     g_f2PI          = static_cast<float>( g_d2PI         );
const float     g_fPIDiv2       = static_cast<float>( g_dPIDiv2      );
const float     g_fPIDiv4       = static_cast<float>( g_dPIDiv4      );
const float     g_fInvPI        = static_cast<float>( g_dInvPI       );
const float     g_fE            = static_cast<float>( g_dE           );
const float     g_fDegToRad     = static_cast<float>( g_dDegToRad    );
const float     g_fRadToDeg     = static_cast<float>( g_dRadToDeg    );
const float     g_fHuge         = static_cast<float>( g_dHuge        );
const float     g_fEpsilon      = static_cast<float>( g_dEpsilon     );
const float     g_fInvEpsilon   = static_cast<float>( g_dInvEpsilon  );
const float     g_fPosInfinity  = static_cast<float>( g_dPosInfinity );
const float     g_fNegInfinity  = static_cast<float>( g_dNegInfinity );
const float     g_fFoV          = static_cast<float>( g_dFoV         );
const float     g_fZNear        = static_cast<float>( g_dZNear       );
const float     g_fZFar         = static_cast<float>( g_dZFar        );

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************
