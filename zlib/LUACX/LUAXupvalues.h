#ifndef __LUAX_UPVALUES_H__
#define __LUAX_UPVALUES_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXupvalues
//
// Description:
//
// LUAXupvalues() finds all upvalues valid in a scope specified by
// a position.
//
// Author: Kevin Senn
// Date:   Friday, September 30th, 2011 @ 02:07:15 PM
//---------------------------------------------------------------------------

#include <COL/COLlist.h>

// Note that this does not get the values of any upvalues - just the names.
void LUAXgetUpvalues(const COLstring& LuaCode, int Position, COLlist<COLstring>* pUpValues);

#endif // end of defensive include
