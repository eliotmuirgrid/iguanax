//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PRMroleCheck.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   29/01/24 3:35 PM
//  ---------------------------------------------------------------------------
#include <PRM/PRMroleCheck.h>
#include <PRM/PRMpermissionUtils.h>

#include <CFG/CFGmapApi.h>

#include <ROL/ROLid.h>

#include <USR/USRuser.h>

#include <COL/COLsplit.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void PRMunionRole(const ROLrole& AgentRole, ROLrole* pUnionRole) {
   COL_FUNCTION(PRMunionRole);
   for(int i = 0; i < AgentRole.m_Role.size(); i++) {
      pUnionRole->m_Role[i] = AgentRole.m_Role[i] || pUnionRole->m_Role[i];
      COL_VAR2(ROLidToString((ROLid) i),  pUnionRole->m_Role[i]);
   }
}

static void PRMresolveRole(bool FromTranslator, const COLarray<COLstring>& AgentTags, const COLarray<COLstring>& ValidRoles, ROLrole* pUnionRole, ROLmap* pRoleMap) {
   COL_FUNCTION(PRMresolveRole);
   if(ValidRoles.size() == 0) {
      if(FromTranslator) {
         COL_TRC("From translator acting on component with no component roles -- default to full permissions");
         *pUnionRole = ROLrole(true);
         return;
      }
      pUnionRole->m_Unsecure = true;
      COL_TRC("Use user-specific roles");
      for(const auto& it : AgentTags) {
         if(pRoleMap->m_Roles.count(it) && !pRoleMap->m_Roles[it].m_ComponentRole) { PRMunionRole(pRoleMap->m_Roles[it], pUnionRole); }
      }
      return;
   }

   COLmap<COLstring, bool> AgentLookup; // for faster lookup in following loop
   for(const auto& it : AgentTags)  { AgentLookup.add(it, true); }
   for(const auto& it : ValidRoles) { if(AgentLookup.count(it)) { PRMunionRole(pRoleMap->m_Roles[it], pUnionRole); } }
}

static bool PRMgetTags(bool FromTranslator, const COLstring& Agent, const COLstring& Target,
                       const COLmap<COLstring, USRuser>* pUserMap, CFGmap* pComponentMap, COLarray<COLstring>* pAgentTags,
                       COLarray<COLstring>* pTargetTags, COLstring* pErr) {
   COL_FUNCTION(PRMgetTags);
   if (Target != "Iguana-Service" && !Target.is_null()) {
      COL_TRC("Checking component specific permissions");
      if (!CFGmapNode(pComponentMap, Target)) {
         *pErr = "Error checking permissions for " + Target + ". The target is not a valid component";
         return false;
      }
      *pTargetTags = PRMtargetComponentTags(pComponentMap, Target);
   }

   if (!PRMagentTags(FromTranslator, Agent, pUserMap, pComponentMap, pAgentTags, pErr)) {
      COL_TRC("Error getting agent tags -- " << *pErr);
      return false;
   }
   return true;
}

void PRMroleChecker(bool FromTranslator, const COLstring& Agent, const COLstring& Target, ROLrole* pAgentRole,
                    COLstring* pError, const COLmap<COLstring, USRuser>* pUserMap, ROLmap* pRoleMap, CFGmap* pComponentMap) {
   COL_FUNCTION(PRMroleChecker);
   COL_VAR2(Agent, Target);
   if(Agent.is_null() || Agent.isWhitespace()) {
      *pError = "No agent defined";
      return;
   }

   COLarray<COLstring> AgentTags, TargetTags;
   if(!PRMgetTags(FromTranslator, Agent, Target, pUserMap, pComponentMap, &AgentTags, &TargetTags, pError)) { return; }

   // Check if TargetTags exist in the role map
   COLarray<COLstring> ValidRoles;
   for(const auto& it : TargetTags) {
      if(pRoleMap->m_Roles.count(it) && pRoleMap->m_Roles[it].m_ComponentRole) {
         COL_VAR("Valid Component Role Found: " << it);
         ValidRoles.push_back(it);
      }
   }

   if(Agent == "admin") {
      const bool Unsecure = ValidRoles.size() == 0;
      COL_VAR2(Agent, Unsecure);
      *pAgentRole = ROLrole(true, Unsecure);
      return;
   }

   PRMresolveRole(FromTranslator, AgentTags, ValidRoles, pAgentRole, pRoleMap);
}

