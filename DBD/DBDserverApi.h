#ifndef __DBD_SERVER_API_H__
#define __DBD_SERVER_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDserverApi
//
// Description:
//
// GRID API with IWEB dependencies.
//
// Author: Eliot Muir
// Date:   Wednesday 21 September 2022 - 06:03PM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class IWEBconfig;

void DBDserverApi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, IWEBconfig* pWebConfig);

#endif // end of defensive include
