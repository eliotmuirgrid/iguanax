#ifndef __LUA_AWAIT_H__
#define __LUA_AWAIT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: LUAawait
//
// Description:
//
// Making Asynchronous operations synchronous using mutex and condition
// variable. wait() and sync() must be called from different threads.
//
// Example:
//
// void updateDatabase(std::weak_ptr<struct LUAsynchronize> pAsync) {
//    // updating DB ...
//    LUAasync(pAsync);
// }
//
// void LDBupdate(...) {
//    COLclosure1<std::weak_ptr<struct LUAsynchronize>>* pFunction = COLnewClosure1(&updateDatabase);
//    bool Success = LUAawait(pFunction, pLoop, ForceKill, 10);
//    if (!Success) {
//       COL_WRN("LUAwait() timed out.");
//    }
// }
//
// Author: John Q
// Date:   Tue 11 Oct 2022 9:32:55 EDT
//---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <memory>

class  SCKloop;
struct LUAsynchronize;

// blocks until LUAasync() is called by the pFunction.
// must be called from Lua worker thread. pFunction will run on main thread.
// returns false if Timeout (in seconds) is set and triggered, true otherwise.
// setting the ForceKill bool reference to true unconditionally aborts LUAawait()
bool LUAawait(COLclosure1<std::weak_ptr<struct LUAsynchronize>>* pFunction, SCKloop* pLoop, bool& ForceKill, int Timeout = 0);
// must be called from the pFunction passed to LUAawait() to cause LUAawait() to return.
void LUAasync(std::weak_ptr<struct LUAsynchronize> pAsync);

#endif // end of defensive include
