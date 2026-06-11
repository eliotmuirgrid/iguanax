//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLserialize.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   29/01/24 4:01 PM
//  ---------------------------------------------------------------------------
#include <ROL/ROLserialize.h>
#include <ROL/ROLid.h>
#include <ROL/ROLrole.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool ROLvarToRole(const COLvar& RoleVar, ROLrole* pRole) {
   COL_FUNCTION(ROLvarToRole);
   if(!RoleVar.isMap()) { return false; }
   pRole->m_Unsecure      = RoleVar["unsecure"].asBool();
   pRole->m_ComponentRole = RoleVar["component_role"].asBool();
   for(const auto& it : RoleVar.map()) {
      if(it.first == "unsecure" || it.first == "component_role") {continue;}
      const ROLid Id = ROLstringToId(it.first);
      COL_VAR3(Id, it.first, it->second);
      if(Id != (ROLid)-1) { pRole->m_Role[Id] = it.second.asBool(); }
      else{return false;}
   }
   return true;
}

bool ROLvarToMap(const COLvar& RoleMapVar, ROLmap* pRoleMap) {
   COL_FUNCTION(ROLvarToMap);
   COL_VAR(RoleMapVar);
   if(!RoleMapVar.isMap()) {
      COL_TRC("Roles are in invalid format");
      return false;
   }
   for(const auto& it : RoleMapVar.map()) {
      COL_VAR2(it.first, it.second.json(1));
      ROLrole Role;
      if(ROLvarToRole(it->second, &Role)) { ROLadd(pRoleMap, it->first, Role); }
      else                                { COL_ERR("Invalid role found -- " << it->second); }
   }
   return true;
}

COLvar ROLmapToVar(const ROLmap& RoleMap) {
   COL_FUNCTION(ROLmapToVar);
   COLvar MapVar;
   if(RoleMap.m_Roles.empty()) { return MapVar; }
   for(const auto& it : RoleMap.m_Roles) {
      MapVar[it->first] = ROLroleToVar(it->second);
   }
   COL_VAR(MapVar.json(1));
   return MapVar;
}

COLvar ROLroleToVar(const ROLrole& Role) {
   COL_FUNCTION(ROLroleToVar);
   COLvar RoleVar;
   for(int i = 0; i < Role.m_Role.size(); i++) { RoleVar[ROLidToString((ROLid) i)] = Role.m_Role[i]; }
   RoleVar["unsecure"]       = Role.m_Unsecure;
   RoleVar["component_role"] = Role.m_ComponentRole;
   COL_VAR(RoleVar.json(1));
   return RoleVar;
}

COLvar ROLroleToVarIguanaOnly(const ROLrole& Role) {
   COL_FUNCTION(ROLroleToVarIguanaOnly);
   COLvar RoleVar;
   for(int i = 0; i < Role.m_Role.size(); i++) {
      const auto Id = (ROLid) i;
      if(ROLisIguanaWide(Id)) { RoleVar[ROLidToString(Id)] = Role.m_Role[i]; }
   }
   RoleVar["unsecure"]       = Role.m_Unsecure;
   RoleVar["component_role"] = Role.m_ComponentRole;
   COL_VAR2(RoleVar.json(1), RoleVar.size());
   return RoleVar;
}