#ifndef __TRN_HOOK_H__
#define __TRN_HOOK_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNhook
//
// Description:
//
// TRNhook class.
//
// Author: Eliot Muir
// Date:   Thursday, November 18th, 2010 @ 02:43:52 PM
//
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>

void TRNhookAnnotationSet(lua_State* L);
void TRNhookAnnotationUnset(lua_State* L);

#endif // end of defensive include
