#ifndef __LUA_TIMEOUT_H__
#define __LUA_TIMEOUT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LULtimeout
//
// Description:
//
// LUAtimeout - routines to record the start of running a script and error
// out if we exceed the expected time.  Moved from ANNtimeout & TNDLtimeout.
//
// For good timeout detection, LUAcheckTimeout() should be called after
// every potentially blocking operation.
//
// Author: Kevin Senn
// Date:   Thursday, April 7th, 2011 @ 04:22:36 PM
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>
#include <time.h>

extern const char* LUAL_TIMEOUT_START;
extern const char* LUAL_TIMEOUT_TARGET;
extern const char* LUAL_TIMEOUT_STRING;
extern const char* LUAL_TIMEOUT_CHECKER;

// The interface for an "additional timeout checker".  See LUAsetTimeoutChecker().
class LUAtimeoutChecker {
public:
   // Should raise a Lua error if the script should time out.
   virtual void checkTimeout(lua_State* L) = 0;
};

// Call just before the script starts.
void LUAbeginScript(lua_State* L);

int LUAsetTimeout(lua_State* L);

// Check regularly throughout script execution.
void LUAcheckTimeout(lua_State* L);

// Sets the Lua environment's "additional timeout checker".
// If a timeout checker is set, then checkTimeout() will be called on
// that checker each time LUAcheckTimeout() is called.
void LUAsetTimeoutChecker(lua_State* L, LUAtimeoutChecker* pChecker);

// unwind_protect(f,g)
//
// Execute pcall(f) and then execute g() with the results from pcall().
// g() is given new timeout restrictions in case f() fails due to a
// timeout/interrupt.  Normally unwind_protect() will return the values
// returned by the call to f().  If an error is thrown from f(),
// unwind_protect() will throw that error after it executes g(); if g()
// throws an error, its error will be thrown by unwind_protect() instead.
//
int LUAunwindProtect(lua_State*);

// These functions handle compile timeouts specifically. The hook that 
// calls the LUAcheckCompileTimeout should be unset after compile is done
void LUAsetCompileTimeout(lua_State* L);
void LUAcheckCompileTimeout(lua_State* L);

#endif // end of defensive include