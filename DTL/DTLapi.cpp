//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DTLapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/03/24 12:27 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DTL/DTLapi.h>
#include <DTL/DTLpinsLoad.h>
#include <DTL/DTLpinsSave.h>
#include <DTL/DTLpinsShift.h>
#include <DTL/DTLtagLoad.h>
#include <DTL/DTLtagSave.h>

COL_LOG_MODULE;

void DTLapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(DTLapi);
	ApiMap.add("/pins/load",        COLnewClosure1(&DTLpinsLoad, pMap, pLoop));
	ApiMap.add("/pins/save",        COLnewClosure1(&DTLpinsSave, pMap, pLoop));
	ApiMap.add("/pins/shift",       COLnewClosure1(&DTLpinsShift, pMap, pLoop));
}

void DTLapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(DBGapiAsync);
   ApiMap.add("/filter_tags/load", &DTLtagLoad);
   ApiMap.add("/filter_tags/save", &DTLtagSave);
}