// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCfieldsRefreshAll
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 27 January 2025 - 11:44AM
// ---------------------------------------------------------------------------
#include <TRNC/TRNCfieldsRefreshAll.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigLoad.h>
#include <CFG/CFGmap.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <SCK/SCKloop.h>
#include <COL/COLthreadPool.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNCrefreshCustomFieldsOnMain(COLmap<COLstring, COLmap<COLstring, COLstring>>* pNewFieldsMap, CFGmap* pMap, COLaddress Address){
   COL_FUNCTION(TRNCrefreshCustomFieldsOnMain);
   for (auto it : pMap->ConfigMap){
      COLstring ComponentId = it.first;
      if (pNewFieldsMap->count(ComponentId)){
         COL_DBG("Updating field values for " << ComponentId);
         COL_TRC("Old fields");
         COL_VAR(it->second.CoreConfig.Fields);
         COL_TRC("New fields");
         COL_VAR((*pNewFieldsMap)[ComponentId]);
         it->second.CoreConfig.Fields = (*pNewFieldsMap)[ComponentId];
      }
   }
   COL_TRC("Refresh of custom fields finished");
   COLrespondSuccess(Address);
}

void TRNCfieldsRefreshAllWorker(COLauto<COLclosure1<COLmap<COLstring, COLmap<COLstring, COLstring>>*>> pCallback) {
   COL_FUNCTION(TRNCfieldsRefreshAllWorker);
   auto* pFieldsMap = new COLmap<COLstring, COLmap<COLstring, COLstring>>();
   const COLstring                                  ConfigDirPath = DIRcompsDir();
   FILdirEnumerator                                 Sniffy(ConfigDirPath + FIL_ANYFILE, true, true);
   COLstring                                        ComponentId;
   while(Sniffy.getFile(ComponentId)) {
      COL_VAR(ComponentId);
      CFGconfig Config;
      if(!CFGconfigLoad(ComponentId, &Config)) {
         COL_TRC("Bad component: " << ComponentId << " - didn't load");
         continue;  // or we could add an empty field map?
      }
      pFieldsMap->addUnique(ComponentId, Config.CoreConfig.Fields);
   }
   pCallback->run(pFieldsMap);
}

// /component/fields/refresh/all
void TRNCfieldsRefreshAll(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop){
   COL_FUNCTION(TRNCfieldsRefreshAll);
	COLauto<COLclosure1<COLmap<COLstring, COLmap<COLstring, COLstring>>*>> pCallback = COLnewClosure1(TRNCrefreshCustomFieldsOnMain, pMap, Request.Address);
	pLoop->threadPool()->scheduleTask(COLnewClosure0(TRNCfieldsRefreshAllWorker, pCallback));
}