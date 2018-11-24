
/* 
GetFilePath.h

A convenient way to search the NVSDK for media files
*/


#ifndef H_GETFILEPATH_H
#define H_GETFILEPATH_H

#include <windows.h>
#include <string>
#include "shared/NV_Common.h"
#include "shared/NV_Error.h"
#include "shared/nvFileSearch.h"


namespace GetFilePath
{
	static std::string strStartPath;

	typedef std::string (*GetFilePathFunction)(const std::string &, bool bVerbose );
	
	// This variable allows us to recursively search for media
	class NVMediaSearch : public NVFileSearch
	{
		public:
		std::string m_strPath;
		virtual bool FileFoundCallback(const WIN32_FIND_DATA& FindData, const std::string& strDir)
		{
			UNREFERENCED_PARAMETER(FindData);
			m_strPath = strDir;
			
			return false;
		}
	};


	static std::string GetModulePath() { return strStartPath; }
	static void        SetModulePath(const std::string &strPath)
	{
		std::string::size_type Pos = strPath.find_last_of("\\", strPath.size());
		if (Pos != strPath.npos)
			strStartPath = strPath.substr(0, Pos);
		else
			strStartPath = ".";
	}

	static void		SetDefaultModulePath()
	{
		DWORD ret;
		TCHAR buf[MAX_PATH];
		ret = GetModuleFileName( NULL, buf, MAX_PATH );		// get name for current module
		if( ret == 0 )
		{
			FMsg("SetDefaultModulePath() GetModuleFileName() failed!\n");
			assert( false );
		}

		SetModulePath( buf );
	}


	
	static std::string GetFullPath(const std::string &strRelativePath)
	{
		char buf[MAX_PATH];
		char *pAdr;
		GetFullPathName(strRelativePath.data(), MAX_PATH, buf, &pAdr);
		return buf;
	}

	//a helper class to save and restore the currentDirectory
	class DirectorySaver
	{
	private:
		char savedDirectory[MAX_PATH];
	public:
		DirectorySaver( )
		{
			// Save current directory
			GetCurrentDirectory(MAX_PATH, savedDirectory);
		}
		~DirectorySaver( )
		{
	  		// return to previous directory
	        SetCurrentDirectory(this->savedDirectory);
		}
	};

    // Recursively searchs the given path until it finds the file. Returns "" if 
    // file can't be found
    static std::string FindMediaFile(const std::string &strFilename, const std::string &mediaPath, bool bVerbose = false )
    {
        WIN32_FIND_DATA findInfo;
        HANDLE hFind;
        std::string result;
    
        
		//save and auto restore the current working directory
		DirectorySaver whenIGoOutOfScopeTheCurrentWorkingDirectoryWillBeRestored;


        // Switch to directory to be removed
        if (!SetCurrentDirectory(mediaPath.data()))
		{
			if( bVerbose )
			{	
				FMsg("FindMediaFile Couldn't SetCurrentDirectory to [%s].  Returning empty string\n", mediaPath.c_str() );
			}

            return "";
		}

        // check if file is in current directory
        FILE *fp;
        fp = fopen(strFilename.data(), "r");
        if (fp)
        {
            fclose(fp);
            return mediaPath + "\\" + strFilename;
        }
		else
		{	
			if( bVerbose )
			{
				// report where the file is NOT
				FMsg("FindMediaFile: File [%s] is not in %s\n", strFilename.c_str(), mediaPath.c_str() );
			}
		}
    
        // if file not in current directory, search for all directories
        // and search inside them until the file is found
        hFind = FindFirstFile("*.*", &findInfo);
        if (hFind == INVALID_HANDLE_VALUE)
            return "";
        
        result = "";
        do
        {
            if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                // Make sure we don't try to delete the '.' or '..' directories
                if ((strcmp(findInfo.cFileName, "..") == 0) || (strcmp(findInfo.cFileName, ".") == 0))
                    continue;
                
				// Add directory to the media path
				// Keep same original file name
                result = FindMediaFile(strFilename, mediaPath + "\\" + findInfo.cFileName, bVerbose );
                if (result != "")
                    break;                    
            }
        } while (FindNextFile(hFind, &findInfo));
    
        FindClose(hFind);
        
        return result;
    }

	//////////////////////////////////////////////////////////////////////////////
	// Gets the full path to the requested file by searching in the MEDIA directory.
    // (note: for a pointer to a 1-parameter version of this function, see below.)
	static std::string GetFilePath(const std::string& strFileName, bool bVerbose )
	{
        // Check if strFilename already points to a file
        FILE *fp;
        fp = fopen(strFileName.data(), "r");
        if (fp)
        {
            fclose(fp);
            return strFileName;
        }

		// You should call SetModulePath before using GetFilePath()
		// If not, it will set the module path for you

		std::string strMediaPath = GetModulePath();
		if( strMediaPath.empty() == true )
		{
			SetDefaultModulePath();
			strMediaPath = GetModulePath();
		}

		// Find the last occurrence of '\' or '/'
		// This has the effect of backing up 4 directories, implying a direct dependence on
		//  the location of the calling .exe in order to start looking for media in the right
		//  place.  This is bad, so a more general search for the first subdirectory \MEDIA 
		//  should be put in place
		// TODO : see above
    	strMediaPath = strMediaPath.substr(0, strMediaPath.find_last_of("\\/"));
	    strMediaPath = strMediaPath.substr(0, strMediaPath.find_last_of("\\/"));
	    strMediaPath = strMediaPath.substr(0, strMediaPath.find_last_of("\\/"));
	    strMediaPath = strMediaPath.substr(0, strMediaPath.find_last_of("\\/"));
        strMediaPath += "\\MEDIA";
			
		std::string result;
        result = FindMediaFile(strFileName, strMediaPath, bVerbose);
           
        if (result != "")
            return result;

        //////////////////// for local shaders /////////////////////////
		strMediaPath = GetModulePath();
        strMediaPath += "\\Shaders";

        result = FindMediaFile(strFileName, strMediaPath, bVerbose);
           
        if (result != "")
            return result;

        //////////////////// for local ../shaders /////////////////////////
		strMediaPath = GetModulePath();
    	strMediaPath = strMediaPath.substr(0, strMediaPath.find_last_of("\\/"));
        strMediaPath += "\\Shaders";

        result = FindMediaFile(strFileName, strMediaPath, bVerbose);
           
        if (result != "")
            return result;

		// If prog gets to here, the find has failed.
		// Return the input file name so other apps can report the failure
		//  to find the file.
		if( bVerbose )
			FMsg("GetFilePath() Couldn't find : %s\n", strFileName.c_str() );

		return strFileName;
	};

    // Use these wrapper functions if you need to pass a pointer [callback] 
    // to a 1-parameter version of the GetFilePath() function:
    static std::string GetFilePath(const std::string& strFileName) {
        return GetFilePath(strFileName, false);
    }
	static std::string GetFilePathVerbose(const std::string& strFileName) {
        return GetFilePath(strFileName, true);
    }

};	 // namespace GetFilePath


#endif
