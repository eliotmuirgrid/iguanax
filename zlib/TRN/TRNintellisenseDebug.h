#ifndef __TRN_INTELLISENSE_DEBUG_H__
#define __TRN_INTELLISENSE_DEBUG_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseDebug
//
// Description:
//
// TRNintellisenseHook - the debug hook for intellisense.
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 10:20:28 PM
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>
#include <COL/COLstring.h>

class HELPobject;

void TRNintellisenseHookSet(lua_State* L);

void TRNintellisenseHook(lua_State* L, lua_Debug* pD);
bool TRNfetchHelpJsonFromFile(lua_State* L, HELPobject* pHelp, COLstring* pHelpFile);

#endif // end of defensive include
