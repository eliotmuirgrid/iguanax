#ifndef __COL_UTILS_H__
#define __COL_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLutils
//
// Description:
//
// COLutils - At the moment it just has COLsleep.  Probably will get
// rid of COLmath.h at some state and put that code in here.
//
// Author: Eliot Muir
// Date:   Wednesday, February 11th, 2009 @ 09:49:46 AM
//---------------------------------------------------------------------------

#include "COLstring.h"
class COLvar;

// This function is correctly implemented under POSIX and Windows
// and will wait for the number of milliseconds given.
//
// Note - there are no hard guarantees on the sleep duration on any platform.
// COLsleep may be interrupted and return early, or sleep slightly longer
// than requested - especially is the machine is under CPU load.
//
// Be aware that the minimum granularity of sleeping on Windows is 15ms.
// Time granularlity on POSIX varies from platform to platform - it may be
// 10ms, 1ms, or some other value. For Linux in particular, the minimum sleep
// duration depends on what HZ setting the kernel was compiled with.
//
// A COLsleep(0) may be useful for yielding the thread, much like an I/O operation.
void COLsleep(int TimeOutInMilliseconds);

// Environment variable expansion function.
//
// Replace all occurances of sub-strings of the form "${VAR}" with the values
// of the environment variables of the same name. If the environment variable is
// not set, it will be replaced with "", an empty string.
//
// Note: This is a SINGLE PASS function that will expand as many ${VARIABLE}s as 
// it finds in the string. It will not attempt to replace any ${VARIABLE}s that arise
// as result of the single-pass expansion.
//
// Also note: this function will has no facility to avoid escaping string literals
// of the form "${VARIABLE}".
COLstring COLexpandEnvironmentVariables(const COLstring&);

// This is similar to COLexpandEnvironmentVariables but expands them using the CachedVars provided.
COLstring COLexpandEnvironmentVariablesUsingCache(const COLstring& Original, const COLvar& CachedVars);

// A simple glob-like matching function.
//
// ListOfGlobs is a whitespace separated list of glob-like patterns.
// If a glob begins with '-', then it is an exclusionary pattern.
// Globs in ListOfGlobs are evaluated from left to right.
//
// For example:
//
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "ABc") == true
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CAB") == true
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "ABCdef") == false
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "ABCde") == true
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CARP") == false
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CAT") == false
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CATS") == true
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "BIRD") == true
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "BIRDs") == false
//  COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CART") == false
//
bool COLglobMatch(const char* ListOfGlobs, const char* ModuleToMatch);

#endif // end of defensive include
