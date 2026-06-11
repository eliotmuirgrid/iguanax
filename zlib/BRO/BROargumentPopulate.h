#ifndef __BRO_ARGUMENT_POPULATE_H__
#define __BRO_ARGUMENT_POPULATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BROargumentPopulate
//
// Description:
//
// New code to get the argument data into the BRO result.
//
// Author: Eliot Muir 
// Date:   Friday 29 November 2024 - 04:09PM
// ---------------------------------------------------------------------------

#include <COL/COLarray.h>
#include <TRN/TRNargumentRequest.h>

class COLvar;
struct lua_State;

void BROargumentPopulate(lua_State* L, int i, COLvar* pResult);

void BROwriteResponses(const COLarray<TRNargRequest>& FullArgumentRequests, COLvar* pResponse);

#endif // end of defensive include
