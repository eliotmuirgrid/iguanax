// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCsourcesAllowable
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 31 July 2024 - 08:36AM
// ---------------------------------------------------------------------------

#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCsourcesAllowable.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void CMPCsetupPotentialSources(COLarray<COLstring>* pSources, const CFGmap& Map, const CFGconfig* pCurrentNode){
   COL_FUNCTION(CMPCsetupPotentialSources);
   auto it = Map.ConfigMap.cbegin();
   COLarray<COLstring> CurrentSources = pCurrentNode->CoreConfig.Sources;
   // Disallow the node and any of its current sources 
   for (it; it != Map.ConfigMap.cend(); it++){
      COLstring CurrentGuid = it->first;
      if (CurrentGuid == pCurrentNode->CoreConfig.Guid) { continue; }
      if (CurrentSources.indexOf(CurrentGuid) != -1)    { continue; }
      pSources->push_back(it->first);
   }
}

// Gross brute force attempt now. For each potential source, try adding the connection and then checking for a cycle.
static void CMPCfindAllowedSources(COLvar* pAllowedSources, const COLarray<COLstring>& PotentialSources, CFGmap& Map, CFGconfig* pCurrentNode){
   COL_FUNCTION(CMPCfindAllowedSources);
   for (int i = 0; i < PotentialSources.size(); i++){
      COLstring SourceGuid = PotentialSources[i];
      CFGconfigAddSource(SourceGuid, pCurrentNode);
      if (!CFGmapHasComponentCycle(&Map, pCurrentNode->CoreConfig.Guid)) {
         COL_TRC(SourceGuid + " is a valid source");
         pAllowedSources->push_back(SourceGuid);
      }  
      CFGconfigRemoveSource(SourceGuid, pCurrentNode);
   }
}

static void CMPCsourcesAllowableImpl(CFGmap* pMap, COLstring Component, COLaddress Address) {
   COL_FUNCTION(CMPCsourcesAllowableImpl);
   CFGmap Temp = *pMap;
   COL_VAR2(pMap, &Temp); // make sure they are not the same address
   CFGconfig* pCurrentNode = CFGmapNode(&Temp, Component);
   if (!pCurrentNode)  { return COLrespondError(Address, "No component exists with name or id " + Component); }
   COL_TRC("Determining all valid sources for component: " << Component);
   COLarray<COLstring> PotentialSources;
   COLvar Out;
   Out["sources"].setArrayType();
   CMPCsetupPotentialSources(&PotentialSources, Temp, pCurrentNode);
   CMPCfindAllowedSources(&Out["sources"], PotentialSources, Temp, pCurrentNode);
   COL_VAR(Out);
   return COLrespondSuccess(Address, Out);
}

// /component/sources/allowable
void CMPCsourcesAllowable(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCsourcesAllowable);
   COLstring Component = COLvarRequiredString(Request.Data, "component");
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCsourcesAllowableImpl, pMap, Component, Request.Address));
}