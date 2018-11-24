//############################################################################
//# ErrorManager - Error / Log handling 
//############################################################################

#pragma once

//############################################################################
//Forward declarations
class CBaseClass;

class CErrorManager {
    public:
        //. Flags used as parameters for Error handler/caster
        static const int FlgError=1;
        static const int FlgWarning=2;
        static const int FlgFatal=4;
        static const int FlgLog=8;

        //Error types (warning - indexes to array)
        static const int TNone=0;
        static const int TMemory=1;
        static const int TFile=2;
        static const int TNoSupport=3;
        static const int TWindow=4;
        static const int TSystem=5;
        static const int TLog=6;

        //v SystemErrors _MUSIA_ byt definovane vsetky typy z enum ERRORS a v definovanom poradi
        typedef struct sErrorInfoTag {
            int Type;    //Typ Chyby
            char *Text;  //Text Chyby
        } sErrorInfo;

    protected:
        static int      ErrorManagerCount;
        char           *LogFileName; //do akeho suboru davat Log
        char           *ErrorText;   //text poslednej chyby
        int             ErrorNumber; //posledne cislo chyby (GetLastError)
        int             ErrorType;   //typ poslednej chyby
        int             ErrorFlags;  //flagy poslednej chyby
        DWORD           StartTickCount; //aky bol tick count, ked system nastartoval

        void DoTerminate(int Flag, int Type);
        void SaveToFile(char *Text);
		char *StrCatHelper(char *Output,char *Buf);
        CErrorManager(char *LogFileName);
        virtual ~CErrorManager();
    public:
        char            CurrFile[2048];
        char            CurrFunc[2048];
        int             CurrLine;
        int             CurrError;
        int             CurrErrorID;

        int     TerminateOnFlags;   //pri akych flagoch chyby ma skoncit aplikacia
        int             Indent;     //odsadenie

        bool RenameLogFile(char *NewName, bool Overwrite = TRUE);
        static CErrorManager *GetErrorManager(char *CallerName,char *LogFile);
        static void DeleteErrorManager(char *CallerName);

/**
	 Function name		: Error
	 Description	    : Error method used by all Error - functions. Uses argument list
	 Return type		: D3_INLINE void
	 Argument			: char *FileName - Name of source file
	 Argument			: int Line - Line in code, where error occured
	 Argument			: char *ClassName - name of class, that generated error
	 Argument			: char *ObjectName - object that generated error
	 Argument			: int Flag - Error flags
	 Argument			: int Type - Error type
	 Argument			: int ErrorID - ID of error (Windows Error)
	 Argument			: char *Format - Format string for error
	 Argument			: va_list ArgumentList - list of arguments.
**/		
    void Error_Private(char *ClassName, char *ObjectName, int Type, bool SaveFile, char *Format, va_list ArgumentList);

    /**
     * Method to set owner (caller) of this 
    **/
    void SetOwner(char *File, int Line, char *Function, int ErrType, int ErrID);
};

extern CErrorManager *ErrMgr;

/*
#ifdef _ERROR
    #undef _ERROR
#endif

#define _ERROR0(FL,TY,EID,FORM) ErrMgr->Error(__FILE__,__LINE__,this,FL,TY,EID,FORM)
#define _ERROR1(FL,TY,EID,FORM,P1) ErrMgr->Error(__FILE__,__LINE__,this,FL,TY,EID,FORM,P1)
#define _ERROR2(FL,TY,EID,FORM,P1,P2) ErrMgr->Error(__FILE__,__LINE__,this,FL,TY,EID,FORM,P1,P2)
#define _ERROR3(FL,TY,EID,FORM,P1,P2,P3) ErrMgr->Error(__FILE__,__LINE__,this,FL,TY,EID,FORM,P1,P2,P3)
#define _ERROR4(FL,TY,EID,FORM,P1,P2,P3,P4) ErrMgr->Error(__FILE__,__LINE__,this,FL,TY,EID,FORM,P1,P2,P3,P4)
#define _ERROR5(FL,TY,EID,FORM,P1,P2,P3,P4,P5) ErrMgr->Error(__FILE__,__LINE__,this,FL,TY,EID,FORM,P1,P2,P3,P4,P5)
*/

#include "noerrmgr.h"
//for errors
void _ERROR(char *Name, char *Class, int Type, char *Format, ...);
void _ERROR(CBaseClass *Obj, int Type, char *Format, ...);

//for warnings
void _WARN(CBaseClass *Obj, int Type, char *Format, ...);
void _WARN(char *Name, char *Class, int Type, char *Format, ...);

//for fatal errors
void _FATAL(CBaseClass *Obj, int Type, char *Format, ...);
void _FATAL(char *Name, char *Class, int Type, char *Format, ...);

//Log globals
void _LOG(CBaseClass *Obj, int Level, char *Format, ...);
void _LOG(char *Name, char *Class, int Level, char *Format, ...);
int _LOGB(CBaseClass *Obj, int Level, char *Format, ...);
int _LOGB(char *Name,char *Class, int Level, char *Format, ...);
void _LOGE(int info, char *Format, ...);
void _LOGE(int info);

#include "errmgr.h"