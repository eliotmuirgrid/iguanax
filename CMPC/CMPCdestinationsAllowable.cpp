// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCdestinationsAllowable
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
#include <CMPC/CMPCdestinationsAllowable.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void CMPCexcludeAllChildren(COLhashmap<COLstring, bool>* pExclude, const COLstring& Component, const CFGmap& Map){
   COL_FUNCTION(CMPCexcludeAllChildren);
   auto it = Map.ConfigMap.cbegin();
   for (it; it != Map.ConfigMap.cend(); it++){
      if (it->second.CoreConfig.Sources.indexOf(Component) != -1) { pExclude->add(it->first, true); }
   }
}

static void CMPCexcludeAllAncestors(COLhashmap<COLstring, bool>* pExclude, const CFGconfig* pComponentNode, CFGmap* pMap){
   COL_FUNCTION(CMPCexcludeAllAncestors);
   for (const auto& it : pComponentNode->CoreConfig.Sources) { 
      pExclude->add(it, true);
      CMPCexcludeAllAncestors(pExclude, CFGmapNode(pMap, it), pMap);
   }
}

static void CMPCdestinationsAllowableImpl(CFGmap* pMap, COLstring Component, COLaddress Address) {
   COL_FUNCTION(CMPCdestinationsAllowableImpl);
   CFGmap Temp = *pMap;
   COL_VAR2(pMap, &Temp); // make sure they are not the same address
   CFGconfig* pCurrentNode = CFGmapNode(&Temp, Component);
   if (!pCurrentNode)  { return COLrespondError(Address, "No component exists with name or id " + Component); }
   COL_TRC("Determining all valid destinations for component: " << Component);
   COLhashmap<COLstring, bool> ExclusionList;
   ExclusionList.add(pCurrentNode->CoreConfig.Guid, true);
   CMPCexcludeAllChildren(&ExclusionList, pCurrentNode->CoreConfig.Guid, Temp);
   CMPCexcludeAllAncestors(&ExclusionList, pCurrentNode, &Temp);
   COLvar Out;
   Out["destinations"].setArrayType();
   for (const auto& it : Temp.ConfigMap){
      if (ExclusionList.count(it->first) == 0) { 
         Out["destinations"].push_back(it->first);
      }
   }
   COL_VAR(Out);
   return COLrespondSuccess(Address, Out);
}

// /component/destinations/allowable
void CMPCdestinationsAllowable(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(CMPCdestinationsAllowable);
   COLstring Component = COLvarRequiredString(Request.Data, "component");
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCdestinationsAllowableImpl, pMap, Component, Request.Address));
}