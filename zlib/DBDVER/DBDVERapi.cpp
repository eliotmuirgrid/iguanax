// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 18 September 2022 - 07:07PM
// ---------------------------------------------------------------------------

#include <DBDVER/DBDVERapi.h>
#include <DBDVER/DBDVERversionNumber.h>
#include <DBDVER/DBDVERversionOnly.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDVERapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap) {
   COL_FUNCTION(DBDVERapi);
   ApiMap.add("/product/version", COLnewClosure1(&DBDVERgetVersionNumber));
   ApiMap.add("/iguana/version",  COLnewClosure1(&DBDBVERversionOnly));

}
