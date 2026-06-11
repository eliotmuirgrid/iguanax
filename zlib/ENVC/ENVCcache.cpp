// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ENVCcache
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue Oct 14 16:37:59 EDT 2025
// ---------------------------------------------------------------------------
#include <ENVC/ENVCcache.h>
#include <COL/COLhashmap.h>
#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static COLhashmap<COLstring, COLstring>& ENVCcache() {
   static COLhashmap<COLstring, COLstring> Cache;
   return Cache;
}

// almost identical to PROfullEnv but without the PRO dependency
extern char **environ;  // Magical POSIX variable.
void ENVCcacheInitOS() {
   char** ppEnv = environ;
   while (*ppEnv != NULL){
      COLstring Line(*ppEnv);
      COLstring Key, Value;
      Line.split(Key, Value, "=");
      ENVCcache()[Key] = Value;
      ppEnv++;
   }
}

void ENVCcacheSet(const COLstring& Name, const COLstring& Value) {
   ENVCcache()[Name] = Value;
}

void ENVCcacheUnset(const COLstring& Name) {
   ENVCcache().removeKey(Name);
}

char* ENVCcacheGet(const char* Name) {
   static COLstring Empty;
   if (ENVCcache().count(Name) == 0) { return nullptr; }
   return ENVCcache()[Name].get_buffer();
}

int ENVCcacheGetEnvInteger(const char* Name, int Default) {
   const char* pValue = ENVCcacheGet(Name);
   int Interval = Default;
   if (pValue) { Interval = atoi(pValue); }
   if (Interval < 0) { Interval = Default; }
   return Interval;
}

char* ENVCgetEnv(const char* Name) {
#ifdef _WIN32
   return ENVCcacheGet(Name);
#else
   return ::getenv(Name);
#endif
}
