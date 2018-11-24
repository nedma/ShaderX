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

#ifndef         FILEHANDLER_H_INCLUDED
#define         FILEHANDLER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        "HandlerBase.hpp"
#include        <fstream>

namespace Logging
{

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  FileHandler
//-----------------------------------------------------------------------------
//
//! Simple file logging Handler
//
//-----------------------------------------------------------------------------
class FileHandler : public HandlerBase
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a default FileHandler
                                FileHandler     ();
    //! \brief  Initializes a FileHandler to write to the given filename
    //! \param  [in] The name of the output file (without extension)
                                FileHandler     ( const std::string& a_fileName );
    //! \brief  Initializes a FileHandler to write to the given filename, with optional append
    //! \param  [in] The name of the output file (without extension)
    //! \param  [in] Specifies append mode
                                FileHandler     ( const std::string& a_fileName
                                                , const bool a_append
                                                );
    //! \brief  Initializes a FileHandler to write to a set of files
    //! \param  [in] The name of the output file (without extension)
    //! \param  [in] The maximum number of bytes to write to any one file
    //! \param  [in] The number of files to use
                                FileHandler     ( const std::string& a_fileName
                                                , const int a_limit
                                                , const int a_count
                                                );
    //! \brief  Initializes a FileHandler to write to a set of files with optional append
    //! \param  [in] The name of the output file (without extension)
    //! \param  [in] The maximum number of bytes to write to any one file
    //! \param  [in] The number of files to use
    //! \param  [in] Specifies append mode
                                FileHandler     ( const std::string& a_fileName
                                                , const int a_limit
                                                , const int a_count
                                                , const bool a_append
                                                );
    //! \brief  Creates a FileHandler from the specified one
    //! \param  [in] The FileHandler to copy from
                                FileHandler     ( const FileHandler& a_other );
    //! \brief  Destructor
    virtual                     ~FileHandler    ();

    //
    // Operations
    //

    //! \brief  Closes the Handler and frees all associated resources
    virtual void                close           ();

    //! \brief  Publishs a LogRecord
    //! \param  [in] Description of the log event
    virtual void                publish         ( const LogRecord& a_record );

    //! \brief  Creates a copy of this Handler
    //! \return A new Handler instance
    virtual IHandler*           clone           () const;

    //
    // Operators
    //

    //! \brief  Sets this FileHandler to the specified one
    //! \param  [in] The FileHandler to copy from
    //! \return The modified FileHandler
    virtual const FileHandler&  operator =      ( const FileHandler& a_other );

private:

    //
    // Operations
    //

    //! \brief  Opens the handler file
    void                        open            ();

    //
    // Private data
    //

    std::string     m_fileName;     //!< Basic log file name
    int             m_limit;        //!< Log file size limit
    int             m_count;        //!< Log file count limit
    bool            m_append;       //!< Flag indicating whether to append to log files
    int             m_currentFile;  //!< Current log file number
    std::ofstream   m_stream;       //!< Log file stream

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

}; // namespace Logging

#endif // FILEHANDLER_H_INCLUDED
