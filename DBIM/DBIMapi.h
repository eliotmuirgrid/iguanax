#ifndef __DBIM_API_H__
#define __DBIM_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMapi
//
// Description:
//
// Web endpoints for Database table import and vdb export features
//
// Author: Vismay Shah
// Date:   Tuesday 10 January 2023 - 09:40AM
// ---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>
class DBdatabaseFactory;
class SCKloop;

void DBIMapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, DBdatabaseFactory* pDatabaseFactory);

#endif // end of defensive include