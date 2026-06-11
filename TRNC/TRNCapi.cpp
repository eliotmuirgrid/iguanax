//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCapi
//
// Description:
//
// APIs for adding, removing and configuring of components.
//
// Author: Shuo Hou
// Date:   Tue August 31 2021
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <TRNC/TRNCapi.h>
#include <TRNC/TRNCcheckInvalid.h>
#include <TRNC/TRNCclearQueue.h>
#include <TRNC/TRNCeditTags.h>
#include <TRNC/TRNCfieldGrammar.h>
#include <TRNC/TRNCfieldsRefreshAll.h>
#include <TRNC/TRNCnames.h>
#include <TRNC/TRNCsetDescription.h>
#include <TRNC/TRNCsetFieldGrammar.h>
#include <TRNC/TRNCsetName.h>
#include <TRNC/TRNCsetTags.h>
#include <USR/USRmap.h>
COL_LOG_MODULE;

void TRNCapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, CFGmap* pMap) {
   COL_FUNCTION(TRNCapi);
   ApiMap.add("/component/check_invalid",      COLnewClosure1(&TRNCcheckInvalid,        pMap));
   ApiMap.add("/component/names",              COLnewClosure1(&TRNCnames,               pMap));
   ApiMap.add("/component/clear_queue",        COLnewClosure1(&TRNCcomponentClearQueue, pMap));
   ApiMap.add("/component/name/set",           COLnewClosure1(&TRNCsetName,             pMap, pLoop));
   ApiMap.add("/component/description/set",    COLnewClosure1(&TRNCsetDescription,      pMap, pLoop));
   ApiMap.add("/component/edit/tags",          COLnewClosure1(&TRNCeditTags,            pMap, pLoop));
   ApiMap.add("/component/tags/set",           COLnewClosure1(&TRNCsetTags,             pMap, pLoop));
   ApiMap.add("/component/fields/refresh/all", COLnewClosure1(&TRNCfieldsRefreshAll,    pMap, pLoop));
}

void TRNCworker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(TRNCworker);
   ApiMap.add("/component/field/grammar/set", &TRNCsetFieldGrammar);
   ApiMap.add("/component/field/grammar",     &TRNCgetFieldGrammar);
}
