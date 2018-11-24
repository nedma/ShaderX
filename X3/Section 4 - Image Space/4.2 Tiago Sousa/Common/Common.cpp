///////////////////////////////////////////////////////////////////////////////////////////////////
//  Proj : ShaderX 3
//  File : Common.cpp
//  Desc : Common data/macros/functions
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <cstdarg>

// output message
void OutputMsg(const char *pTitle, const char *pText, ...) 
{
  va_list vl;
  char pStr[1024];

  // get variable-argument list
  va_start(vl, pText);
  // write formatted output
  vsprintf(pStr, pText, vl);
  // just do it!
  MessageBox(NULL,pStr, pTitle,MB_OK|MB_ICONINFORMATION);

  va_end(vl);
 }
