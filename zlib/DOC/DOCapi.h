#ifndef __DOC_API_H__
#define __DOC_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCapi
//
// Description:
//
// The API for document management system
//
// Author: Eliot Muir 
// Date:   Sunday 24 August 2025 - 03:09PM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class BLUAinstanceManager;
class SCKloop;

void DOCapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

void DOCapiA(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, BLUAinstanceManager* pBluaManager);

#endif // end of defensive include
