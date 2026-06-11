//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 12:55p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SDBS/SDBSapi.h>
#include <SDBS/SDBSdetailSearch.h>
#include <SDBS/SDBSdetailSearchState.h>
#include <SDBS/SDBSretrieve.h>
#include <SDBS/SDBSretrieveState.h>
COL_LOG_MODULE;

void SDBSlogSearch(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
                   COLhashmap<COLstring, SDBSretrieveState>* pLogSearchStateMap, SCKloop* pLoop) {
   COL_FUNCTION(SDBSlogSearch);
   ApiMap.add("/logs/retrieve",         COLnewClosure1(&SDBSretrieve, pLogSearchStateMap, pLoop));
   ApiMap.add("/logs/retrieve/results", COLnewClosure1(&SDBSretrieveResults, pLogSearchStateMap));
}

void SDBSlogDetailSearch(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
                         COLhashmap<COLstring, SDBSdetailSearchItem>* pLogDetailSearchMap, SCKloop* pLoop) {
   COL_FUNCTION(SDBSlogDetailSearch);
   ApiMap.add("/log/detail/search/start", COLnewClosure1(&SDBSdetailSearchStart, pLogDetailSearchMap, pLoop));
   ApiMap.add("/log/detail/search/poll",  COLnewClosure1(&SDBSdetailSearchPoll, pLogDetailSearchMap));
}