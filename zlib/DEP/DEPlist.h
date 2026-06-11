#ifndef __DEP_LUA_H__
#define __DEP_LUA_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DEPdeprecated
//
// Description:
//
// Read the list of deprecated Lua functions.
//
// Author: Tyler Brown 
// Date:   Tuesday 08 April 2025 - 07:01AM
// ---------------------------------------------------------------------------
class COLstring;
class COLwebRequest;

#include <COL/COLarray.h>

void DEPreadLua(COLarray<COLstring> *pLines);
void DEPlua(const COLwebRequest& Request);

#endif // end of defensive include
