//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNisValidLuaName
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, December 21st, 2010 @ 06:58:44 AM
//
//---------------------------------------------------------------------------
#include "TRNconstant.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool TRNisValidLuaName(const COLstring& LuaString){
   return strspn(LuaString.c_str(), TRN_ALPHANUMERIC_CHARACTERS) == LuaString.size();
}
