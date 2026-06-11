// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ENVapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 23 March 2023 - 11:50AM
// ---------------------------------------------------------------------------
#include <ENV/ENVapi.h>
#include <ENV/ENVcatalogue.h>
#include <ENV/ENVdelete.h>
#include <ENV/ENVset.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void ENVapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(ENVapiAsync);
   ApiMap.add("/environment/all",          &ENVcatalogue);
   ApiMap.add("/environment/set_local",    &ENVsetLocal);
   ApiMap.add("/environment/delete",       &ENVdelete);
}
