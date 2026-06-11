#ifndef __LEDI_UTILS_H__
#define __LEDI_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEDIutils
//
// Description:
//
// Function to resolve a vmd file.
//
// Author: Vismay Shah 
// Date:   Thursday 3 October 2024 - 09:36AM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>
struct lua_State;

COLstring LEDIresolveVmdFile(lua_State* L);
#endif // end of defensive include
