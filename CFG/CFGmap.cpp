//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGmap
//
// Description: Storage struct for components
//
// Author: Shuo Hou
// Date:   Tuesday, Aug 31th, 2021
//---------------------------------------------------------------------------
#include "CFGmap.h"
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <CFG/CFGmapApi.h>
COL_LOG_MODULE;

CFGmap* p_g_ComponentMap;

void CFGmapLookupSet(CFGmap* pComponentMap){
   COL_FUNCTION(CFGmapLookupSet);
   p_g_ComponentMap = pComponentMap;
}

const CFGmap& CFGmapGet(){
   COL_FUNCTION(CFGmapGet);
   return *p_g_ComponentMap;
}

COLostream& operator<<(COLostream& Stream, const CFGmap& Map){
   Stream << newline;
   COLmap<COLstring, CFGconfig>::const_iterator Itr = Map.ConfigMap.cbegin();
   for (; Itr != Map.ConfigMap.cend(); Itr++) {
      Stream << Itr->first << " => ";
      Stream << Itr->second;
      Stream << newline;
   }
   return Stream;
}

void CFGconvertNameToIdIfNeeded(const COLvar& NameArray, CFGmap* pMap, COLarray<COLstring>* pComponentGuids, COLarray<COLstring>* pBadComponents) {
   COL_FUNCTION(CFGconvertNameToIdIfNeeded);
   COLarray<COLstring>& ComponentGuids = *pComponentGuids;
   COLarray<COLstring>& BadComponents  = *pBadComponents;
   COLhashmap<COLstring,bool> ComponentsChecked;
   for (const auto& Component : NameArray.array()) {
      if(!ComponentsChecked.count(Component)){
         const CFGconfig* pTransNode = CFGmapNode(pMap, Component);
         if (!pTransNode) {
            pBadComponents->push_back(Component);
            continue;
         }
         if (pMap->ConfigMap.count(Component)) {
            ComponentGuids.push_back(Component);
         } else {
            ComponentGuids.push_back(pTransNode->CoreConfig.Guid);
         }
         ComponentsChecked[Component] = true;
      } else {COL_TRC(Component << " has already been checked!");}
   }
}

bool CFGcomponentId(const COLvar& Data, COLstring* pComponentId, COLstring* pErr){
   COL_FUNCTION(CFGcomponentId);
   COLstring Component = COLvarRequiredString(Data, "component");
   CFGmap Map = CFGmapGet();
   CFGconfig* pTransNode = CFGmapNode(&Map, Component);
   if (!pTransNode){
      *pErr = "No component exists with name or id " + Component;
      return false;
   } else {
      *pComponentId = pTransNode->CoreConfig.Guid;
      return true;
   }
}
