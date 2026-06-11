#ifndef __DBDUP_API_H__
#define __DBDUP_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDUPapi
//
// Description:
//
// Api to check for new releases of Iguana
//
// Author: Vismay Shah 
// Date:   Monday 14 August 2023 - 10:15AM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void DBDUPapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
