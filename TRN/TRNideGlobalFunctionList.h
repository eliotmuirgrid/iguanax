#ifndef __TRN_IDE_TREE_H__
#define __TRN_IDE_TREE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideTree
//
// Description:
//
// Attempt to make a new implemenation of showing the data in node view in the IDE.
//
// Author: Eliot Muir 
// Date:   Friday 29 November 2024 - 11:59AM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
class COLwebRequest;
class SCKloop;
struct TRNjobQueue;

void TRNideGlobalFunctionList(const COLwebRequest& Request, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues);

#endif // end of defensive include
