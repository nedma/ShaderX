// SC-SyntaxScanner.cpp: implementation of the CSyntaxScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SC-SyntaxScanner.h"
//#include "MGR-MemoryManager.h"
#include "SC-MString.h"

static const char SyntaxTerminals[]={'-','.','!','$','%','^','*','(',')','+','=','|','\\','{','}','[',']',
                      ':',';','\"','\'','<','>',',','.','?','/','#','&'};

#define IS(X) ((Flags & X)==X)

CSyntaxScanner::CSyntaxScanner() {
    Source = NULL;
    IsOwner = FALSE;
    Position = 0;
    Flags = 0;
    Size = 0;
    LastTokenString = new CMString;
}

CSyntaxScanner::~CSyntaxScanner() {
    if (IsOwner && Source!=NULL) MemMgr->FreeMem(Source);
    delete LastTokenString;
}

void CSyntaxScanner::SetSource(char *Source, UINT Size, bool IsOwner) {
    if (IsOwner && Source!=NULL) MemMgr->FreeMem(Source);   //free previous source...
    this->Source = Source;
    this->IsOwner = IsOwner;
    this->Size = Size;
    Position = 0;
}

//parses to very simple tokens
int CSyntaxScanner::GetNextLowLevelToken() {
    *LastTokenString = "";
    LastTokenChar = 0;
    LastTokenInt = 0;
    LastTokenFloat = 0;
    if (Position<0 || Position>Size) {
        LastToken = SS_FOUND_EOF;
        LastTokenStart = Size;
        LastTokenEnd = Size+1;
        return LastToken;
    }

    if (Source[Position]=='_' || isalpha(Source[Position])) {   //word
        LastToken = SS_FOUND_WORD;
        LastTokenStart = Position;
        while (Position<Size && (Source[Position]=='_' || isalnum(Source[Position]))) Position++;
        LastTokenEnd = Position;
        char *tmp = (char *)MemMgr->AllocMem(LastTokenEnd-LastTokenStart+1, 0);
        memcpy(tmp,&Source[LastTokenStart], LastTokenEnd-LastTokenStart);
        tmp[LastTokenEnd-LastTokenStart] = 0;
        *LastTokenString = tmp;
        MemMgr->FreeMem(tmp);
        return LastToken;
    }
    if (isdigit(Source[Position])) {
        LastToken = SS_FOUND_INTEGER;
        LastTokenStart = Position;
        while (Position<Size && isdigit(Source[Position])) Position++;
        LastTokenEnd = Position;
        char *tmp = (char *)MemMgr->AllocMem(LastTokenEnd-LastTokenStart+1, 0);
        memcpy(tmp,&Source[LastTokenStart], LastTokenEnd-LastTokenStart);
        tmp[LastTokenEnd-LastTokenStart] = 0;
        LastTokenInt = _atoi64(tmp);
        *LastTokenString = tmp;
        MemMgr->FreeMem(tmp);
        return LastToken;
    }
    LastToken = SS_FOUND_TERMINAL;
    LastTokenStart = Position;
    LastTokenChar = Source[Position];
    *LastTokenString = LastTokenChar;
    LastTokenEnd = Position+1;
    Position++;
    return LastToken;
}

//parses to higher level tokens
int CSyntaxScanner::GetNextToken() {
    int ret = GetNextLowLevelToken();
    if (ret== SS_FOUND_EOF) return ret;

    if (ret== SS_FOUND_TERMINAL) {
        if (LastTokenChar == ' ') {
            LastToken = SS_FOUND_SPACE;
            return LastToken;
        }
        if (LastTokenChar == '\t') {
            LastToken = SS_FOUND_TAB;
            return LastToken;
        }
        if (LastTokenChar == 0x0D) {
            UINT OldPos = Position;
            if (GetNextLowLevelToken() == SS_FOUND_TERMINAL && LastTokenChar== 0x0A) {
                LastToken = SS_FOUND_EOLN;
                return LastToken;
            }
            Position = OldPos;
            LastToken = SS_FOUND_EOLN;
            return LastToken;
        }
        if (LastTokenChar == 0x0A) {
            LastToken = SS_FOUND_EOLN;
            return LastToken;
        }
        if (LastTokenChar == '/') {
            UINT OldPos = Position;
            if (GetNextLowLevelToken() == SS_FOUND_TERMINAL && LastTokenChar== '/') {
                CMString result;
                while (1) {
                    UINT tmp = Position;
                    int tRet = GetNextLowLevelToken();
                    if (tRet == SS_FOUND_EOF) break;
                    if (tRet == SS_FOUND_TERMINAL && (LastTokenChar == 0x0D || LastTokenChar == 0x0A)) {
                        Position = tmp;
                        break;
                    }
                    result+=*LastTokenString;
                }
                *LastTokenString = result;
                LastToken = SS_FOUND_REMARK;
                return LastToken;
            }
            Position = OldPos;
        }
        if (LastTokenChar == '\"') {
            UINT OldPos = Position;
            bool Invalid = FALSE;
            CMString result;
            while (1) {
                int tRet = GetNextLowLevelToken();
                if (tRet == SS_FOUND_EOF) {
                    Invalid = TRUE;
                    break;
                }
                if (tRet == SS_FOUND_TERMINAL && (LastTokenChar == 0x0D || LastTokenChar == 0x0A)) {   //newline cannot be in string...
                    Invalid = TRUE;
                    break;
                }
                if (tRet == SS_FOUND_TERMINAL && LastTokenChar == '\"') {
                    break;
                }
                result+=*LastTokenString;
            }
            if (Invalid) {
                Position = OldPos;
            } else {
                *LastTokenString = result;
                LastToken = SS_FOUND_STRING;
                return LastToken;
            }
        }
        if (LastTokenChar == '+' || LastTokenChar == '-') { //test if this is number...
            UINT OldPos = Position;
            int Sign = (LastTokenChar == '+') ? 1 : -1;
            int tRet = GetNextLowLevelToken();
            if (tRet == SS_FOUND_INTEGER) {
                UINT OldPos2 = Position;
                __int64 BeforeDecimal = LastTokenInt;
                tRet = GetNextLowLevelToken();
                if (LastTokenChar != '.') { //decimal
                    Position = OldPos2;
                    LastTokenInt = (__int64)Sign * BeforeDecimal;
                    LastTokenFloat = (int)BeforeDecimal;
                    LastToken = SS_FOUND_INTEGER;
                    return LastToken;
                } else { //possible float
                    tRet = GetNextLowLevelToken();
                    if (LastToken != SS_FOUND_INTEGER) {
                        Position = OldPos2;
                        LastTokenInt = (__int64)Sign * BeforeDecimal;
                        LastTokenFloat = (int)BeforeDecimal;
                        LastToken = SS_FOUND_INTEGER;
                        return LastToken;
                    } else {
                        double FractPart = (int)LastTokenInt;
                        while (FractPart>=1) FractPart = FractPart / 10;
                        LastTokenFloat = ((double)Sign)*(((double)BeforeDecimal) + FractPart);
                        LastTokenInt = (__int64)LastTokenFloat;
                        LastToken = SS_FOUND_FLOAT;
                        return LastToken;
                    }
                }
            }
            Position = OldPos;
        }
        for (int i=0; i<sizeof(SyntaxTerminals); i++) {
            if (LastTokenChar==SyntaxTerminals[i]) {
                LastToken = SS_FOUND_TERMINAL;
                return LastToken;
            }
        }
        LastToken = SS_FOUND_UNKNOWN;
        return LastToken;
    } else if (ret == SS_FOUND_INTEGER) {
        UINT OldPos = Position;
        __int64 BeforeDecimal = LastTokenInt;
        int tRet = GetNextLowLevelToken();
        if (LastTokenChar != '.') { //decimal
            Position = OldPos;
            LastTokenInt = BeforeDecimal;
            LastTokenFloat = (int)BeforeDecimal;
            LastToken = SS_FOUND_INTEGER;
            return LastToken;
        } else { //possible float
            tRet = GetNextLowLevelToken();
            if (LastToken != SS_FOUND_INTEGER) {
                Position = OldPos;
                LastTokenInt = BeforeDecimal;
                LastTokenFloat = (int)BeforeDecimal;
                LastToken = SS_FOUND_INTEGER;
                return LastToken;
            } else {
                double FractPart = (double)LastTokenInt;
                while (FractPart>=1) FractPart = FractPart / 10;
                LastTokenFloat = ((double)BeforeDecimal) + FractPart;
                LastTokenInt = (__int64)LastTokenFloat;
                LastToken = SS_FOUND_FLOAT;
                return LastToken;
            }
        }
    }
    if (ret == SS_FOUND_WORD) {
        LastToken = SS_FOUND_WORD;
        return LastToken;
    }
    LastToken = SS_FOUND_UNKNOWN;
    return LastToken;
}

int CSyntaxScanner::ScanNext() {
    int ret = 0;
    while (1) {
        ret = GetNextToken();
        if (ret == SS_FOUND_TERMINAL) break;
        if (ret == SS_FOUND_WORD) break;
        if (ret == SS_FOUND_EOLN && IS(SS_FLAG_NEWLINE)) break;
        if (ret == SS_FOUND_EOF) break;
        if (ret == SS_FOUND_REMARK && IS(SS_FLAG_REMARK)) break;
        if (ret == SS_FOUND_INTEGER) break;
        if (ret == SS_FOUND_STRING) break;
        if (ret == SS_FOUND_SPACE && IS(SS_FLAG_SPACE)) break;
        if (ret == SS_FOUND_TAB && IS(SS_FLAG_TAB)) break;
        if (ret == SS_FOUND_FLOAT) break;
    }
    return ret;
}

CMString CSyntaxScanner::AsString() {
    CMString ret;
    switch (LastToken) {
        case SS_FOUND_TERMINAL : {
            ret.Format("%c",LastTokenChar);
        } break;
        case SS_FOUND_WORD : {
            ret = *LastTokenString;
        } break;
        case SS_FOUND_EOLN : {
            ret = "\n";
        } break;
        case SS_FOUND_EOF : {
            ret = "";
        } break;
        case SS_FOUND_REMARK : {
            ret = *LastTokenString;
        } break;
        case SS_FOUND_INTEGER : {
            ret.Format("%i",LastTokenInt);
        } break;
        case SS_FOUND_STRING : {
            if (IS(SS_FLAG_STRINGINQUOTES)) ret = "\""+ *LastTokenString+"\"";
            else ret = *LastTokenString;
        } break;
        case SS_FOUND_SPACE : {
            ret = " ";
        } break;
        case SS_FOUND_TAB : {
            ret = "\t";
        } break;
        case SS_FOUND_FLOAT : {
            ret.Format("%f",LastTokenFloat);
        } break;
    }
    return ret;
}

char CSyntaxScanner::AsChar() {
    char ret = 0;
    switch (LastToken) {
        case SS_FOUND_TERMINAL : {
            ret=LastTokenChar;
        } break;
        case SS_FOUND_WORD : {
            ret = LastTokenString->GetAt(0);
        } break;
        case SS_FOUND_EOLN : {
            ret = '\n';
        } break;
        case SS_FOUND_REMARK : {
            ret = '/';
        } break;
        case SS_FOUND_STRING : {
            if (IS(SS_FLAG_STRINGINQUOTES)) ret = '\"';
            else ret = LastTokenString->GetAt(0);
        } break;
        case SS_FOUND_SPACE : {
            ret = ' ';
        } break;
        case SS_FOUND_TAB : {
            ret = '\t';
        } break;
    }
    return ret;
}

__int64 CSyntaxScanner::AsInt64() {
    __int64 ret = 0;
    switch (LastToken) {
        case SS_FOUND_INTEGER : {
            ret = LastTokenInt;
        } break;
        case SS_FOUND_FLOAT : {
            ret = (__int64)LastTokenFloat;
        } break;
    }
    return ret;
}

double CSyntaxScanner::AsFloat() {
    double ret = 0.0;
    switch (LastToken) {
        case SS_FOUND_INTEGER : {
            ret = (double)LastTokenInt;
        } break;
        case SS_FOUND_FLOAT : {
            ret = LastTokenFloat;
        } break;
    }
    return ret;
}
