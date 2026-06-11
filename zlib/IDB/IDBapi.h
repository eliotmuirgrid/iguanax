#ifndef __IDB_API_H__
#define __IDB_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBapi
//
// Description:
//
// WEB API for DB APIs.
//
// Author: Eliot Muir
// Date:   Sunday 18 September 2022 - 01:23PM
// ---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>
class IDBdatabaseConfig;
class SCKloop;

void IDBapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, IDBdatabaseConfig* pDatabaseConfig);

#endif // end of defensive include
