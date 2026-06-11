#ifndef __TRN_CALL_MAIN_H__
#define __TRN_CALL_MAIN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcallMain
//
// Description:
//
// TRNcallMain - call the Lua function
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 03:27:57 PM
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>
class COLstring;

// Returns true if the script was successfully run.
// Returns false if there was an error, and populates pError with the error message.
bool TRNcallFunctionWithSampleData(lua_State* L, const COLstring& SampleFunction, const COLstring& SampleData, COLstring* pError);
#endif // end of defensive include
