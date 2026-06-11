//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SRCHapi
//
// Description: Implementation
//
// Author: Eliot Muir
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SRCH/SRCHapi.h>
#include <SRCH/SRCHfind.h>
#include <SRCH/SRCHreplaceAll.h>
COL_LOG_MODULE;

void SRCHapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   COL_FUNCTION(SRCHapi);
   ApiMap.add("/ide/search/find",    &SRCHfind);
   ApiMap.add("/ide/search/replace", &SRCHreplaceAll);
}