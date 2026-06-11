#ifndef __TRN_API_H__
#define __TRN_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNapi
//
// Description:
//
// API for translator
//
// Author: Eliot Muir
// Date:   Monday 24 April 2023 - 11:13AM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>
struct TRNjobQueue;
class SCKloop;

void TRNapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues);
void TRNworker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
