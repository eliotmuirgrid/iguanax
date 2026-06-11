#ifndef __DBDVER_API_H__
#define __DBDVER_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERapi
//
// Description:
//
// API for product version.
//
// Author: Eliot Muir
// Date:   Sunday 18 September 2022 - 07:07PM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

void DBDVERapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap);

#endif // end of defensive include
