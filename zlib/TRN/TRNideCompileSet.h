#ifndef __TRN_IDE_COMPILE_SET_H__
#define __TRN_IDE_COMPILE_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideCompileSet
//
// Description:
//
// Api to set that a compile is required for a translator job queue. Will create
// job queue with no tasks registered if it is not already present.
//
// Author: Vismay Shah 
// Date:   Tuesday 09 May 2023 - 01:17PM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
class COLwebRequest;
struct TRNjobQueue;
class SCKloop;

void TRNideCompileSet(const COLwebRequest& Request, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues);

#endif // end of defensive include
