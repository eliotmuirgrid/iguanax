#ifndef __TRN_AUTO_COMPLETE_H__
#define __TRN_AUTO_COMPLETE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNautoComplete
//
// Description:
//
// New routine to obtain auto complete information.
//
// Author: Eliot Muir 
// Date:   Friday 06 March 2026 - 03:23PM
// ---------------------------------------------------------------------------

class TRNautoCompleteInfo;
struct lua_State;

// This will collect all the options which exist for a given table
void TRNautoCompleteChildOptions(lua_State* L, TRNautoCompleteInfo* pInfo);

#endif // end of defensive include
