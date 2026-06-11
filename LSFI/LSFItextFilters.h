#ifndef __LSFI_TEXT_FILTERS_H__
#define __LSFI_TEXT_FILTERS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LSFItextFilters
//
// Description:
//
// Lua wrapper for C functions which make up the "filter" static keyword in Lua.
//
// Author: Eric Mulvaney
// Date:   Monday, June 13th, 2011 @ 05:53:28 PM
//
//---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <LUAL/LUALutils.h>

struct lua_State;
class COLfilter;

// This convenience method is provided as a lua method - it wraps a COLfilter and applies it to a string or stream on the lua stack
// The COLfilter can be created with a call to 'new' - in the worst case, a created closure will take ownership of it and free it appropriately
int LSFIapplyTextFilter(lua_State* L, COLauto<COLfilter> pFilter);   // assumes i == 1 is the argument
int LSFIapplyTextFilter(lua_State* L, int i, COLauto<COLfilter> pFilter);

class LSFItextFilter {
public:
   // Takes ownership of the passed in filter, so it's safe to use with 'new' filters
   explicit LSFItextFilter(COLauto<COLfilter>& pFilter);
   static int reader(lua_State*);
private:
   COLauto<COLfilter> m_pFilter;
   bool m_Finished;
   int filterChunk(lua_State* L, int Args);
};

void LSFItextFiltersAddNamespace(lua_State*);

#endif // end of defensive include
