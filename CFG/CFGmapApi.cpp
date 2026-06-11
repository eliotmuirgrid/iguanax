//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGmapApi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wed 13 Jul 2022 15:13:05 EDT
//---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigLoad.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void CFGremoveMissingConnections(COLmap<COLstring, CFGconfig>& ConfigMap, CFGconfig* pConfig) {
   COL_FUNCTION(CFGremoveMissingConnections);
   for(int j = pConfig->CoreConfig.Sources.size() - 1; j >= 0; j--) {
      COLstring Source = pConfig->CoreConfig.Sources[j];
      COL_VAR2(j, Source);
      if(!ConfigMap.count(Source)) {
         COL_TRC("Removing missing source " << Source << " from " << pConfig->CoreConfig.Guid);
         pConfig->CoreConfig.Sources.remove(j);
      }
   }
}

void CFGremoveDeletedSources(CFGmap* pMap) {
   COL_FUNCTION(CFGremoveDeletedSources);
   COLmap<COLstring, CFGconfig>& ConfigMap = pMap->ConfigMap;
   for(auto i = ConfigMap.begin(); i != ConfigMap.end(); ++i) {
      COL_TRC("Process component " << i->first);
      CFGremoveMissingConnections(ConfigMap, &i->second);
   }
}

void CFGmapAddComponent(CFGmap* pMap, const CFGconfig& Config) {
   COL_FUNCTION(CFGmapAddComponent);
   COL_VAR(Config);
   pMap->ConfigMap.addUnique(Config.CoreConfig.Guid, Config);  // hopefully shallow copy constructor works
   COL_TRC("Added new component");
}

bool CFGmapAddComponentFromGuid(CFGmap* pMap, const COLstring& Component, const COLstring& ServerGuid) {
   COL_FUNCTION(CFGmapAddComponentFromGuid);
   COL_VAR(Component);
   CFGconfig NewConfig;
   if(!CFGconfigLoad(Component, &NewConfig)) {
      COL_TRC("Bad component - didn't load");
      return false;
   }
   NewConfig.m_HostServerGuid    = ServerGuid;
   NewConfig.m_Initialized       = true;
   NewConfig.CoreConfig.Guid     = Component;  // TODO - revisit - should we error out if the GUID doesn't match
   NewConfig.Status.LastActivity = 0;
   CFGmapAddComponent(pMap, NewConfig);
   return true;
}

bool CFGmapremoveComponent(CFGmap* pMap, const COLstring& ComponentGuid) {
   COL_FUNCTION(CFGmapremoveComponent);
   COL_VAR(ComponentGuid);
   if(!pMap->ConfigMap.erase(ComponentGuid)) { return false; };
   COL_TRC("Removed: " + ComponentGuid);
   return true;
}

CFGconfig* CFGmapNode(CFGmap* pMap, const COLstring& Component) {
   COL_FUNCTION(CFGmapNode);
   if(Component.is_null() || Component.isWhitespace()) { return nullptr; }
   if(pMap->ConfigMap.count(Component) != 0) { return &pMap->ConfigMap[Component]; }
   for(const auto& it : pMap->ConfigMap) {
      if(it.second.CoreConfig.Name == Component) {
         COL_TRC("Found name match -- " << Component << " to " << it.first);
         return &pMap->ConfigMap[it.first];
      }
   }
   COL_TRC("Component -- " + Component + " -- not found in map");
   return nullptr;
}

bool CFGmapNameExists(CFGmap* pMap, const COLstring& Name, CFGconfig* pComponent) {
   COL_FUNCTION(CFGmapNameExists);
   COLmap<COLstring, CFGconfig>::iterator it = pMap->ConfigMap.begin();
   for(; it != pMap->ConfigMap.end(); it++) { 
      if(it->second.CoreConfig.Name == Name) { 
         *pComponent = it->second;
         return true; 
      }
   }
   return false;
}

bool CFGmapNameExists(CFGmap* pMap, const COLstring& Guid, const COLstring& Name) {
   COL_FUNCTION(CFGmapNameExists);
   COLmap<COLstring, CFGconfig>::iterator it = pMap->ConfigMap.begin();
   for(; it != pMap->ConfigMap.end(); it++) { if(it->second.CoreConfig.Name == Name && it->first != Guid) { return true; }}
   return false;
}

void CFGmapNames(CFGmap* pMap, COLvar* pOutput){
   COL_FUNCTION(CFGmapNames);
   COLvar& Output = (*pOutput);
   Output.reset();
   Output.setMapType();
   COLmap<COLstring, CFGconfig>::iterator it = pMap->ConfigMap.begin();
   for(; it != pMap->ConfigMap.end(); it++) {
      if(!it->second.CoreConfig.Name.is_null()){
         Output[it->second.CoreConfig.Name] = true;
      }
   }
}

void CFGmapRemoveNodesFromServer(CFGmap* pMap, const COLstring& ServerGuid) {
   COL_FUNCTION(CFGmapRemoveNodesFromServer);
   COLmap<COLstring, CFGconfig>::iterator it = pMap->ConfigMap.begin();
   while(it != pMap->ConfigMap.end()) {
      if(it.value().m_HostServerGuid == ServerGuid) {
         COL_TRC("Removing " + it->first);
         it = pMap->ConfigMap.erase(it);
      } else { it++; }
   }
}

static bool CFGmapDetectCycle(CFGmap* pMap, const COLstring& ComponentId, COLhashmap<COLstring, bool>& Seen) {
   COL_FUNCTION(CFGmapDetectCycle);
   COL_VAR2(ComponentId, Seen.size());
   const auto& Sources = pMap->ConfigMap[ComponentId].CoreConfig.Sources;
   for (const auto & Source : Sources) {
      COL_VAR(Source);
      if (Seen.count(Source)) { return true; }
      Seen[Source] = true;
      if (CFGmapDetectCycle(pMap, Source, Seen)) { return true; }
      Seen.removeKey(Source);
   }
   return false;
}

bool CFGmapHasComponentCycle(CFGmap* pMap, const COLstring& ComponentId) {
   COL_FUNCTION(CFGmapHasComponentCycle);
   COLhashmap<COLstring, bool> Seen;
   return CFGmapDetectCycle(pMap, ComponentId, Seen);
}
