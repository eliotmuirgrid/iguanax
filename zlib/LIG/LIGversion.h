#ifndef __LIG_VERSION_H__
#define __LIG_VERSION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LIGversion
//
// Description:
//
// Strcut containing Iguana version info and lua API to retrieve version info.
//
// Author: Vismay Shah
// Date:   Thursday 29 December 2022 - 02:51PM
// ---------------------------------------------------------------------------
struct lua_State;

struct LIGiguanaVersion{
   int Major;
   int Minor;
   int Build;
};

void LIGspecifyIguanaVersion(int Major, int Minor, int Build);
int LIGversion(lua_State* L);


#endif // end of defensive include
