///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ipc.h
// Purpose:     wrapper around different wxIPC classes implementations
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.04.02
// RCS-ID:      $Id: ipc.h,v 1.1 2002/04/15 18:24:45 VZ Exp $
// Copyright:   (c) 2002 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// Set wxUSE_DDE_FOR_IPC to 1 to use DDE for IPC under Windows. If it is set to
// 0, or if the platform is not Windows, use TCP/IP for IPC implementation

#if !defined(wxUSE_DDE_FOR_IPC)
    #ifdef __WXMSW__
        #define wxUSE_DDE_FOR_IPC 1
    #else
        #define wxUSE_DDE_FOR_IPC 0
    #endif
#endif // !defined(wxUSE_DDE_FOR_IPC)

#if !defined(__WINDOWS__)
    #undef wxUSE_DDE_FOR_IPC
    #define wxUSE_DDE_FOR_IPC 0
#endif

#if wxUSE_DDE_FOR_IPC
    #define wxConnection    wxDDEConnection
    #define wxServer        wxDDEServer
    #define wxClient        wxDDEClient

    #include "wx/dde.h"
#else // !wxUSE_DDE_FOR_IPC
    #define wxConnection    wxTCPConnection
    #define wxServer        wxTCPServer
    #define wxClient        wxTCPClient

    #include "wx/sckipc.h"
#endif // wxUSE_DDE_FOR_IPC/!wxUSE_DDE_FOR_IPC

