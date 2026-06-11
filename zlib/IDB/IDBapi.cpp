// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 18 September 2022 - 01:23PM
// ---------------------------------------------------------------------------

#include <IDB/IDBapi.h>
#include <IDB/IDBget.h>
#include <IDB/IDBset.h>

#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void IDBapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, IDBdatabaseConfig* pDatabaseConfig){
   COL_FUNCTION(IDBapi); //for tracing
   ApiMap.add("/settings/db/get",          COLnewClosure1(&IDBgetDriverPaths, pDatabaseConfig));
   ApiMap.add("/settings/db/set",          COLnewClosure1(&IDBsetDriverPaths, pLoop, pDatabaseConfig));
}
