#ifndef __TRN_ARGUMENT_FORMAT_H__
#define __TRN_ARGUMENT_FORMAT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentFormat
//
// Description:
//
// TRNargumentFormat - format arguments into and out of functions for annotations.
//
// Author: Eliot Muir
// Date:   Saturday, November 20th, 2010 @ 02:38:37 PM
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>

class COLstring;
class COLostream;

void TRNargumentsFormat(lua_State* L, int Start, int End, const COLstring& FunctionAddress, bool IsReturn, COLostream& Stream);
#endif // end of defensive include   