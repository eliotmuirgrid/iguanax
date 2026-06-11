#ifndef __SDBT_SEARCH_H__
#define __SDBT_SEARCH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTsearch
//
// Description:
//
// Test function for searching for message ID?
//
// Author: John Qi
// Date:   Monday 27 February 2023 - 03:25PM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SCKloop;

void SDBTseachMessageId(COLint64 Id, int FD, SCKloop* pLoop);

#endif // end of defensive include
