#include "stdafx.h"
#include "MGR-ErrorManager.h"
#include "baseclass.h"
#include "time.h"

static CErrorManager::sErrorInfo SystemErrors[]={
    {CErrorManager::TNone,"%s"},
    {CErrorManager::TMemory,"%s Memory: %s\n"},
    {CErrorManager::TFile,"%s File: %s\n"},
    {CErrorManager::TNoSupport,"%s 'No support': %s\n"},
    {CErrorManager::TWindow,"%s Window: %s\n"},
    {CErrorManager::TSystem,"%s System: %s\n"},
    {CErrorManager::TLog,"%s: %s\n"},
};

static char sErrorDef[]="ERR  ";    //slovo "ERROR"
static char sWarningDef[]="WARN ";  //slovo "WARNING"
static char sFatalDef[]="FERR ";    //slovo "FATAL Error"
static char sLogDef[]="LOG  ";      //slovo "LOG"
static char sNoneDef[]="     ";      //slovo "none"

int CErrorManager::ErrorManagerCount=0;
CErrorManager *ErrMgr=NULL;

CErrorManager *CErrorManager::GetErrorManager(char *CallerName,char *LogFile) {
    if (ErrorManagerCount==0) {
        ErrMgr = new CErrorManager(LogFile);
        ErrorManagerCount=1;
    } else {
        ErrorManagerCount++;
        _LOG("Err","MGR",D3_DV_GRP0_LEV0,"%s called request for ErrorManager. Reference count is %d",CallerName,ErrorManagerCount);
    }
    return ErrMgr;
}

void CErrorManager::DeleteErrorManager(char *CallerName) {
    if ((ErrMgr==NULL) || (ErrorManagerCount==0)) return;
    else {
        ErrorManagerCount--;
        _LOG("Err","MGR",D3_DV_GRP0_LEV0,"%s wanted to delete ErrorManager. Reference count is %d",CallerName,ErrorManagerCount);
        if (ErrorManagerCount==0) {
            delete ErrMgr;
            ErrMgr=NULL;
        }
    }
}

CErrorManager::CErrorManager(char *LogFileName) {
    ErrorText = NULL;
    TerminateOnFlags = FlgError | FlgFatal;
    Indent = 0;
    if (LogFileName!=NULL) {
        this->LogFileName=(char*)calloc(strlen(LogFileName)+1,1);
        strcpy(this->LogFileName,LogFileName);
        char tmp[1024];
        char tim[64];
        char dat[64];
        StartTickCount = GetTickCount();
        _strdate(dat);
        _strtime(tim);
        sprintf(tmp,"--[NEW_LOG_SESSION]-[%s %s]-----------------------------------------\n\n",dat,tim);
        SaveToFile(tmp);
        SaveToFile("LOG Err.MGR Starting up...\n");
    } else {
        this->LogFileName=NULL;
    }
}

CErrorManager::~CErrorManager() {
    if (LogFileName!=NULL) {
        char tim[64];
        char dat[64];
        _strdate(dat);
        _strtime(tim);
        _LOG("Err","MGR",D3_DV_GRP0_LEV0,"Shutting down...");
		char tmp[1024];
        sprintf(tmp,"\n\n--[END_LOG_SESSION]-[%s %s]-----------------------------------------\n\n",dat,tim);
        SaveToFile(tmp);
        free(LogFileName);
    }
    if (ErrorText!=NULL) free(ErrorText);
}

void CErrorManager::SaveToFile(char *Text) {
    if (LogFileName!=NULL) {
        FILE *f;
        f = fopen(LogFileName,"at");
        fprintf(f,Text);
        fflush(f);
        fclose(f);
    }
}

void CErrorManager::DoTerminate(int Flag, int Type) {
    if ((Flag & TerminateOnFlags)!=0) {
		char *tmp = NULL;
        if (ErrorNumber>0) {
            LPVOID lpMsgBuf;
            FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, ErrorNumber,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
							(LPTSTR) &lpMsgBuf, 0, NULL);
			tmp = StrCatHelper(tmp,"An termination event caused the application to suicide.\n\nA goodbye letter has text:\n");
			tmp = StrCatHelper(tmp,ErrorText);
			tmp = StrCatHelper(tmp,"\n\nWindows shouts this error : \n");
			tmp = StrCatHelper(tmp,(char *)lpMsgBuf);
			tmp = StrCatHelper(tmp,"\n\nRemember, suicide is painless and try again :)");
            LocalFree( lpMsgBuf );
        } else {
			tmp = StrCatHelper(tmp,"An termination event caused the application to suicide.\n\nA goodbye letter has text:\n");
			tmp = StrCatHelper(tmp,ErrorText);
			tmp = StrCatHelper(tmp,"\n\nRemember, suicide is painless and try again :)");
        }
        ::MessageBox(NULL,tmp,"TERMINATION EVENT...",MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
        MessageBeep(MB_ICONHAND);
		free(tmp);
        if (Flag == FlgFatal) {
            //Application terminate in case of fatal error
            _exit(0);
        }
        //Application terminate in case of normal error
        exit(0);
    }
}


bool CErrorManager::RenameLogFile(char *NewName, bool Overwrite) {
    if (Overwrite) {
        remove(NewName);
    }
    int result = rename(this->LogFileName, NewName);
    if (result == 0) {
        char tmp[512];
        strncpy(tmp,this->LogFileName,511);
        tmp[511]=0;
        this->LogFileName = (char *)realloc(this->LogFileName, strlen(NewName)+1);
        strcpy(this->LogFileName,NewName);
        _LOG("Err","MGR",D3_DV_GRP0_LEV0,"Renamed log file from %s to %s",tmp,this->LogFileName);
        return TRUE;
    } else {
        return FALSE;
    }
}

char *CErrorManager::StrCatHelper(char *Output,char *Buf) {
	if (Output==NULL) {
		Output = (char *)malloc(1);
		Output[0]=0;
	}
	if (Buf==NULL) return Output;
	Output = (char *)realloc(Output,strlen(Output)+strlen(Buf)+1);
	strcat(Output,Buf);
	return Output;
}

void CErrorManager::Error_Private(char *ClassName, char *ObjectName, int Type, bool SaveFile, char *Format, va_list ArgumentList) {
    int result = -1;
	char *FormatedError = NULL;
	char *FormatedResult = NULL;
	char *buf = NULL;
	int size = 256;

	if (Format==NULL) return;
    while (result==-1) {
		size = size+256;
		buf = (char *)realloc(buf,size);
		result = _vsnprintf(buf, size, Format, ArgumentList);
	}
	if (result!=-1) {
		FormatedError = (char *)malloc(strlen(buf)+1);
		strcpy(FormatedError,buf);
	}
	free(buf);

    //concat those prefixes / suffixes
	char *temp = NULL;
    DWORD TickCount = GetTickCount() - StartTickCount;
    char tmpTime[256];
    sprintf(tmpTime,"%012u ",TickCount);
    temp = StrCatHelper(temp,tmpTime);
    if (CurrError == FlgFatal) temp = StrCatHelper(temp,sFatalDef);
    else if (CurrError == FlgError) temp = StrCatHelper(temp,sErrorDef);
    else if (CurrError == FlgWarning) temp = StrCatHelper(temp,sWarningDef);
    else if (CurrError == FlgLog) temp = StrCatHelper(temp,sLogDef);
    else temp = StrCatHelper(temp,sNoneDef);

    //preparing indent
    char *ind = (char*)malloc(Indent+1);
    memset(ind,' ',Indent);
    for (int i=0; i<Indent; i++) {
        ind[i]=' ';
    }
    ind[Indent] = 0;
    temp = StrCatHelper(temp,ind);
    free(ind);


    if (SaveFile) {
        char buf[50];
        temp = StrCatHelper(temp,CurrFile);
        temp = StrCatHelper(temp,"(");
        temp = StrCatHelper(temp,itoa(CurrLine,buf,10));
        temp = StrCatHelper(temp,")");
        temp = StrCatHelper(temp," @");
        temp = StrCatHelper(temp,CurrFunc);
        temp = StrCatHelper(temp," ");
    }

    if (ObjectName!=NULL && ClassName!=NULL) {
		temp = StrCatHelper(temp,ObjectName);
		temp = StrCatHelper(temp,".");
		temp = StrCatHelper(temp,ClassName);
	} else if (ObjectName==NULL && ClassName!=NULL) {
        temp = StrCatHelper(temp,".");
		temp = StrCatHelper(temp,ClassName);
	} else if (ObjectName!=NULL && ClassName==NULL) {
		temp = StrCatHelper(temp,ObjectName);
	}

    result = -1;
    buf = NULL;
    size = 256;
    while (result<0) {
        size = size+512;
        buf = (char *)realloc(buf,size);
        result = _snprintf(buf,size,SystemErrors[Type].Text,temp,FormatedError);
    }
    if (result>=0) {
        FormatedResult = (char *)malloc(strlen(buf)+1);
        strcpy(FormatedResult,buf);
    }
    free(buf);
    ErrorText = (char *)realloc(ErrorText,strlen(FormatedResult)+1);
    strcpy(ErrorText,FormatedResult);
	free(FormatedResult);
	free(FormatedError);
    ErrorFlags=CurrError;
    ErrorType=Type;
	ErrorNumber = CurrErrorID;
    SaveToFile(ErrorText);
    DoTerminate(CurrError,Type);
}

void CErrorManager::SetOwner(char *File, int Line, char *Function, int ErrType, int ErrID) {
    strcpy(this->CurrFile, File);
    strcpy(this->CurrFunc, Function);
    CurrLine = Line;
    CurrError = ErrType;
    CurrErrorID = ErrID;
}

#include "noerrmgr.h"

//_LOG methods
void _LOG(CBaseClass *Obj, int Level, char *Format, ...) {
    #ifdef D3_DEBUG_VERBOSE
        if (((Level&D3_DEBUG_VERBOSE_LEVEL)==Level) && (ErrMgr!=NULL)) {
            va_list argList;
            va_start(argList, Format);
            if (Obj!=NULL) {
                char *ClassName = NULL;
                ClassName = (char *)malloc(Obj->GetClsName(ClassName,0));
                Obj->GetClsName(ClassName,Obj->GetClsName(ClassName,0));
                ErrMgr->Error_Private(ClassName, Obj->GetObjName(), CErrorManager::TLog, false, Format, argList);
                free(ClassName);
            } else {
                ErrMgr->Error_Private(NULL, NULL, CErrorManager::TLog, false, Format, argList);
            }
            va_end(argList);
        }
    #endif
}

void _LOG(char *Name, char *Class, int Level, char *Format, ...) {
    #ifdef D3_DEBUG_VERBOSE
        if (((Level&D3_DEBUG_VERBOSE_LEVEL)==Level) && (ErrMgr!=NULL)) {
            va_list argList;
            va_start(argList, Format);
	        ErrMgr->Error_Private(Class, Name, CErrorManager::TLog, false, Format, argList);
            va_end(argList);
        }
    #endif
}

typedef struct sBlockLogTag {
    int OldIndent;
    char       *Name;
    char       *Class;
} sBlockLog;

int _LOGB(CBaseClass *Obj, int Level, char *Format, ...) {
    #ifdef D3_DEBUG_VERBOSE
        if (((Level&D3_DEBUG_VERBOSE_LEVEL)==Level) && (ErrMgr!=NULL)) {
            sBlockLog *res = (sBlockLog *)calloc(sizeof(sBlockLog),1);
            if (Obj!=NULL) {
                va_list argList;
                va_start(argList, Format);
                res->Class = (char *)malloc(Obj->GetClsName(NULL,0));
                res->Name = (char *)malloc(Obj->GetObjName(NULL,0));
                Obj->GetClsName(res->Class,Obj->GetClsName(NULL,0));
                Obj->GetObjName(res->Name,Obj->GetObjName(NULL,0));
                ErrMgr->Error_Private(res->Class, res->Name, CErrorManager::TLog, false, Format, argList);
                va_end(argList);
            } else {
                va_list argList;
                va_start(argList, Format);
                ErrMgr->Error_Private(res->Class, res->Name, CErrorManager::TLog, false, Format, argList);
                va_end(argList);
            }
            res->OldIndent = ErrMgr->Indent;
            ErrMgr->Indent = ErrMgr->Indent+2;
            return (int)res;
        } else {
            return 0;
        }
    #else
        return 0;
    #endif
}

int _LOGB(char *Name,char *Class, int Level, char *Format, ...) {
    #ifdef D3_DEBUG_VERBOSE
        if (((Level&D3_DEBUG_VERBOSE_LEVEL)==Level) && (ErrMgr!=NULL)) {
            sBlockLog *res = (sBlockLog *)calloc(sizeof(sBlockLog),1);
            va_list argList;
            va_start(argList, Format);
            if (Class!=NULL) {
                res->Class = (char *)malloc(strlen(Class)+1);
                strcpy(res->Class,Class);
            }
            if (Name!=NULL) {
                res->Name = (char *)malloc(strlen(Name)+1);
                strcpy(res->Name,Name);
            }
            ErrMgr->Error_Private(res->Class, res->Name, CErrorManager::TLog, false, Format, argList);
            va_end(argList);
            res->OldIndent = ErrMgr->Indent;
            ErrMgr->Indent = ErrMgr->Indent+2;
            return (int)res;
        } else {
            return 0;
        }
    #else
        return 0;
    #endif
}

void _LOGE(int info, char *Format, ...) {
    #ifdef D3_DEBUG_VERBOSE
        if ((info!=0) && (ErrMgr!=NULL)) {
            sBlockLog *res = (sBlockLog *)info;
            ErrMgr->Indent = res->OldIndent;
            va_list argList;
            va_start(argList, Format);
            ErrMgr->Error_Private(res->Class, res->Name, CErrorManager::TLog, false, Format, argList);
            va_end(argList);
            if (res->Class!=NULL) free(res->Class);
            if (res->Name!=NULL) free(res->Name);
            free(res);
        }
    #endif
}


void _LOGE(int info) {
    #ifdef D3_DEBUG_VERBOSE
        if ((info!=0) && (ErrMgr!=NULL)) {
            sBlockLog *res = (sBlockLog *)info;
            ErrMgr->Indent = res->OldIndent;
            if (res->Class!=NULL) free(res->Class);
            if (res->Name!=NULL) free(res->Name);
            free(res);
        }
    #endif
}


void _ERROR(CBaseClass *Obj, int Type, char *Format, ...) {
    if (ErrMgr!=NULL) {
        ErrMgr->CurrError = CErrorManager::FlgError;
        if (Obj!=NULL) {
            char *ClassName = NULL;
            ClassName = (char *)malloc(Obj->GetClsName(ClassName,0));
            Obj->GetClsName(ClassName,Obj->GetClsName(ClassName,0));
            va_list argList;
            va_start(argList, Format);
            ErrMgr->Error_Private(ClassName, Obj->GetObjName(), Type, true, Format, argList);
            free(ClassName);
        } else {
            va_list argList;
            va_start(argList, Format);
            ErrMgr->Error_Private(NULL, NULL, Type, true, Format, argList);
            va_end(argList);
        }
    }
}

void _ERROR(char *Name, char *Class, int Type, char *Format, ...) {
    if (ErrMgr!=NULL) {
        ErrMgr->CurrError = CErrorManager::FlgError;
        va_list argList;
        va_start(argList, Format);
        ErrMgr->Error_Private(Class, Name, Type, true, Format, argList);
        va_end(argList);
    }
}

//for warnings
void _WARN(CBaseClass *Obj, int Type, char *Format, ...) {
    if (ErrMgr!=NULL) {
        ErrMgr->CurrError = CErrorManager::FlgWarning;
        if (Obj!=NULL) {
            char *ClassName = NULL;
            ClassName = (char *)malloc(Obj->GetClsName(ClassName,0));
            Obj->GetClsName(ClassName,Obj->GetClsName(ClassName,0));
            va_list argList;
            va_start(argList, Format);
            ErrMgr->Error_Private(ClassName, Obj->GetObjName(), Type, true, Format, argList);
            free(ClassName);
        } else {
            va_list argList;
            va_start(argList, Format);
            ErrMgr->Error_Private(NULL, NULL, Type, true, Format, argList);
            va_end(argList);
        }
    }
}

void _WARN(char *Name, char *Class, int Type, char *Format, ...) {
    if (ErrMgr!=NULL) {
        ErrMgr->CurrError = CErrorManager::FlgWarning;
        va_list argList;
        va_start(argList, Format);
        ErrMgr->Error_Private(Class, Name, Type, true, Format, argList);
        va_end(argList);
    }
}

//for fatal errors
void _FATAL(CBaseClass *Obj, int Type, char *Format, ...) {
    if (ErrMgr!=NULL) {
        ErrMgr->CurrError = CErrorManager::FlgFatal;
        if (Obj!=NULL) {
            char *ClassName = NULL;
            ClassName = (char *)malloc(Obj->GetClsName(ClassName,0));
            Obj->GetClsName(ClassName,Obj->GetClsName(ClassName,0));
            va_list argList;
            va_start(argList, Format);
            ErrMgr->Error_Private(ClassName, Obj->GetObjName(), Type, true, Format, argList);
            free(ClassName);
        } else {
            va_list argList;
            va_start(argList, Format);
            ErrMgr->Error_Private(NULL, NULL, Type, true, Format, argList);
            va_end(argList);
        }
    }
}

void _FATAL(char *Name, char *Class, int Type, char *Format, ...) {
    if (ErrMgr!=NULL) {
        ErrMgr->CurrError = CErrorManager::FlgFatal;
        va_list argList;
        va_start(argList, Format);
        ErrMgr->Error_Private(Class, Name, Type, true, Format, argList);
        va_end(argList);
    }
}
