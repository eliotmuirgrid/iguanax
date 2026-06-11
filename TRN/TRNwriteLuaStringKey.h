#ifndef __TRN_WRITE_LUA_STRING_KEY_H__
#define __TRN_WRITE_LUA_STRING_KEY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNwriteLuaStringKey
//
// Description:
//
// Helper
//
// Author: Eliot Muir
// Date:   Tuesday, December 21st, 2010 @ 06:58:44 AM
//
//---------------------------------------------------------------------------

#include <COL/COLstring.h>

void TRNwriteLuaStringKey(const COLstring& Key, COLostream& Stream, bool UseColon = false);

#endif // end of defensive include
