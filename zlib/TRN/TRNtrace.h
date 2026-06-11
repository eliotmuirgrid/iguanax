#ifndef __TRN_TRACE_H__
#define __TRN_TRACE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtrace
//
// Description:
//
// TRNtrace - utilities to replace functions so that we can trace how they are called.
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 04:27:12 PM
//
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>

int TRNtraceC(lua_State* L); // replace C function

#endif // end of defensive include
