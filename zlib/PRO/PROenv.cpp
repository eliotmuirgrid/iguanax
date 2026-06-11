//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROenv
//
// Description:
//
// Interact with environment variables in a cross-platform manner 
//
// Author: Nicolas Lehman
// Date:   18/07/19
//---------------------------------------------------------------------------

#include "PROenv.h"
#include <stdlib.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void PROsetenv(const COLstring& Key, const COLstring& Value) {
#ifdef _WIN32
   COL_TRC("Setting environment variable");
   _putenv_s(Key.c_str(), Value.c_str());
   COL_VAR2(Key, Value);
#else
   COL_TRC("Setting environment variable");
   setenv(Key.c_str(), Value.c_str(), 1);
   COL_VAR2(Key, Value);
#endif
}

void PROunsetenv(const COLstring& Key){
   COL_FUNCTION(PROunsetenv);
   COL_VAR(Key);
#ifndef _WIN32
   unsetenv(Key.c_str());
#else
   PROsetenv(Key.c_str(), "");
#endif
}


COLstring PROgetenv(const COLstring& Key) {
   COLstring Value;
#ifdef _WIN32
   COL_TRC("Getting environment variable");
   Value = getenv(Key.c_str());
   COL_VAR2(Key, Value);
#else
   COL_TRC("Getting environment variable");
   Value = getenv(Key.c_str());
   COL_VAR2(Key, Value);
#endif
   return Value;
}

extern char **environ;  // Magical POSIX variable.
// This function will give a complete map of all the environmental variables.
void PROfullEnv(COLvar* pOut){
   COL_FUNCTION(PROfullEnv);
   COLvar& Cat = *pOut;
   Cat.setMapType();
   char** ppEnv = environ;
   while (*ppEnv != NULL){
      COLstring Line = *ppEnv;
      COLstring Key, Value;
      Line.split(Key, Value, "=");
      COL_VAR(*ppEnv);
      Cat[Key] = Value;
      ppEnv++;
   }
}