// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PRMpermissionUtils
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 02 January 2024 - 02:52PM
// ---------------------------------------------------------------------------

#include <PRM/PRMpermissionUtils.h>
#include <USR/USRuser.h>
#include <CFG/CFGmap.h>

#include <COL/COLmap.h>
#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool PRMcheckForUser(const COLmap<COLstring, USRuser>* pUserMap, const COLstring& Username, COLstring* pError){
   if (!pUserMap->count(Username)) {
      COL_TRC("Could not find user " + Username);
      *pError = "Could not find user " + Username;
      return false;
   }
   return true;
}

static bool PRMcheckForComponent(const CFGmap* pComponentMap, const COLstring& ComponentId, COLstring* pError){
   if (!pComponentMap->ConfigMap.count(ComponentId)) {
      COL_TRC("Could not find component " + ComponentId);
      *pError = "Could not find component " + ComponentId;
      return false;
   }
   return true;
}

bool PRMagentTags(bool FromTranslator, const COLstring& Agent, const COLmap<COLstring, USRuser>* pUserMap, const CFGmap* pComponentMap,
                  COLarray<COLstring>* pAgentTags, COLstring* pError){
   COL_FUNCTION(PRMagentTags);
   if (Agent.is_null()) {
      *pError = "No agent defined";
      return false;
   }
   if (FromTranslator){
      COL_TRC("Agent was a component");
      if (!PRMcheckForComponent(pComponentMap, Agent, pError)) { return false; }
      for(const auto& it : pComponentMap->ConfigMap[Agent].CoreConfig.Tags) { pAgentTags->push_back(it); }
      return true;
   } 
   COL_TRC("Agent was a user");
   if (!PRMcheckForUser(pUserMap, Agent, pError)) { return false; }
   *pAgentTags = (*pUserMap)[Agent].roles();
   return true;
}

COLstring PRMtargetComponentName(const CFGmap* pComponentMap, const COLstring& Target){
   // COL_FUNCTION(PRMtargetComponentName);
   COLstring TargetName;
   if (pComponentMap->ConfigMap.count(Target)){
      TargetName = pComponentMap->ConfigMap[Target].CoreConfig.Name;
   }
   return TargetName;
}

COLarray<COLstring> PRMtargetComponentTags(const CFGmap* pComponentMap, const COLstring& Target){
   // COL_FUNCTION(PRMtargetComponentTags);
   COL_VAR(Target);
   if (pComponentMap->ConfigMap.count(Target)){
      return pComponentMap->ConfigMap[Target].CoreConfig.Tags;
   }
   return COLarray<COLstring>();
}
