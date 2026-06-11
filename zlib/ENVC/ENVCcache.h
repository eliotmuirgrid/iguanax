#ifndef __ENVC_CACHE_H__
#define __ENVC_CACHE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ENVCcache
//
// Description:
//
// Environment variable cache in memory to avoid excessive getenv() calls
//
// Author: John Q
// Date:   Tue Oct 14 16:37:59 EDT 2025
// ---------------------------------------------------------------------------

class COLstring;

// loads OS env vars. Iguana env vars must be loaded separately.
void  ENVCcacheInitOS();
void  ENVCcacheSet  (const COLstring& Name, const COLstring& Value);
void  ENVCcacheUnset(const COLstring& Name);
char* ENVCcacheGet  (const char* Name);

// get env var as a position integer
// returns Default if not set or value <= 0
int ENVCcacheGetEnvInteger(const char* Name, int Default);

// replacement for getenv
// calls getenv on POSIX, uses the cache on Windows. See IX-4245
// don't forget to initialize the cache using ENVCcacheInitOS() and ENVCcacheSet(). See ENVstartup()
char* ENVCgetEnv(const char* Name);

#endif // end of defensive include
