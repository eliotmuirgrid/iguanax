//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testPermissions.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   07/02/24 12:32 PM
//  ---------------------------------------------------------------------------
#include "testPermissions.h"

#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>

#include <PRM/PRMroleCheck.h>

#include <ROL/ROLrole.h>

#include <UNIT/UNITapp.h>

#include <USR/USRuser.h>

#include <COL/COLsplit.h>
#include <COL/COLmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void resetRole(ROLrole* pRole) { *pRole = ROLrole(); }

static ROLmap setupRoleMap() {
   ROLmap RoleMap;
   ROLrole Critical = ROLrole(true, false, true);
   ROLrole Dev = ROLrole(true, false, false);

   ROLrole Support = ROLrole(false, false, false);
   Support.m_Role[ROLid::VIEW_LOGS] = true;

   ROLadd(&RoleMap, "#critical", Critical);
   ROLadd(&RoleMap, "#dev", Dev);
   ROLadd(&RoleMap, "#support", Support);
   return RoleMap;
}
static COLarray<COLstring> setupTags(const COLstring& Tags) {
   COLarray<COLstring> Vec;
   COLsplit(&Vec, Tags, " ");
   return Vec;
}

static COLmap<COLstring, USRuser> setupUsers() {
   COLmap<COLstring, USRuser> UserMap;
   USRuser Admin;
   Admin.setUsername("admin");

   USRuser DevUser;
   DevUser.setUsername("devUser");
   DevUser.setRoles(setupTags("#dev"));

   USRuser SupportUser;
   SupportUser.setUsername("supportUser");
   SupportUser.setRoles(setupTags("#support"));

   USRuser CriticalUser;
   CriticalUser.setUsername("criticalUser");
   CriticalUser.setRoles(setupTags("#dev #critical"));

   UserMap.add(Admin.username(), Admin);
   UserMap.add(DevUser.username(), DevUser);
   UserMap.add(SupportUser.username(), SupportUser);
   UserMap.add(CriticalUser.username(), CriticalUser);
   return UserMap;
}

static CFGmap setupComponents() {
   CFGmap ComponentMap;
   CFGconfig Component1;
   Component1.CoreConfig.Guid = "Component1";
   Component1.CoreConfig.Name = "Component1";
   Component1.CoreConfig.Tags = setupTags("#dev");

   CFGconfig Component2;
   Component2.CoreConfig.Guid = "Component2";
   Component2.CoreConfig.Name = "Component2";

   CFGconfig CriticalComponent;
   CriticalComponent.CoreConfig.Guid = "CriticalComponent";
   CriticalComponent.CoreConfig.Name = "CriticalComponent";
   CriticalComponent.CoreConfig.Tags = setupTags("#critical");

   CFGmapAddComponent(&ComponentMap, Component1);
   CFGmapAddComponent(&ComponentMap, Component2);
   CFGmapAddComponent(&ComponentMap, CriticalComponent);
   return ComponentMap;
}

static void testSupport() {
   COL_FUNCTION(testSupport);
   COLmap<COLstring, USRuser> Users = setupUsers();
   CFGmap Components = setupComponents();
   ROLmap Roles = setupRoleMap();
   ROLrole RoleCheck;
   COLstring Error;
   PRMroleChecker(false, "supportUser", "Component2", &RoleCheck, &Error, &Users, &Roles, &Components);
   UNIT_ASSERT(RoleCheck.m_Unsecure);
   for(int i = 0; i < RoleCheck.m_Role.size(); i++) {
      if(i == (int) ROLid::VIEW_LOGS) { UNIT_ASSERT(RoleCheck.m_Role[i]); }
      else                            { UNIT_ASSERT(!RoleCheck.m_Role[i]); }

   }
   resetRole(&RoleCheck);

   PRMroleChecker(false, "supportUser", "CriticalComponent", &RoleCheck, &Error, &Users, &Roles, &Components);
   for(const auto& Role : RoleCheck.m_Role) { UNIT_ASSERT(!Role); }
   resetRole(&RoleCheck);
}

static void testCritical() {
   COL_FUNCTION(testCritical);
   COLmap<COLstring, USRuser> Users = setupUsers();
   CFGmap Components = setupComponents();
   ROLmap Roles = setupRoleMap();
   ROLrole RoleCheck;
   COLstring Error;
   PRMroleChecker(false, "admin", "Component1", &RoleCheck, &Error, &Users, &Roles, &Components);
   UNIT_ASSERT(RoleCheck.m_Unsecure);
   for(const auto& Role : RoleCheck.m_Role) { UNIT_ASSERT(Role); }
   resetRole(&RoleCheck);

   PRMroleChecker(false, "devUser", "CriticalComponent", &RoleCheck, &Error, &Users, &Roles, &Components);
   for(const auto& Role : RoleCheck.m_Role) { UNIT_ASSERT(!Role); }
   resetRole(&RoleCheck);

   PRMroleChecker(false, "criticalUser", "CriticalComponent", &RoleCheck, &Error, &Users, &Roles, &Components);
   for(const auto& Role : RoleCheck.m_Role) { UNIT_ASSERT(Role); }
}

static void testUnsecure() {
   COL_FUNCTION(testUnsecure);
   COLmap<COLstring, USRuser> Users = setupUsers();
   CFGmap Components = setupComponents();
   ROLmap Roles = setupRoleMap();
   ROLrole RoleCheck;
   COLstring Error;
   PRMroleChecker(false, "devUser", "Component1", &RoleCheck, &Error, &Users, &Roles, &Components);
   UNIT_ASSERT(RoleCheck.m_Unsecure);
   for(const auto& Role : RoleCheck.m_Role) { UNIT_ASSERT(Role); }
}

static void testAdminPermissions() {
   COL_FUNCTION(testAdminPermissions);
   COLmap<COLstring, USRuser> Users = setupUsers();
   CFGmap Components = setupComponents();
   ROLmap Roles = setupRoleMap();
   ROLrole RoleCheck;
   COLstring Error;
   PRMroleChecker(false, "admin", "Component1", &RoleCheck, &Error, &Users, &Roles, &Components);
   UNIT_ASSERT(RoleCheck.m_Unsecure);
   for(const auto& Role : RoleCheck.m_Role) { UNIT_ASSERT(Role); }
   PRMroleChecker(false, "admin", "Component2", &RoleCheck, &Error, &Users, &Roles, &Components);
   for(const auto& Role : RoleCheck.m_Role) { UNIT_ASSERT(Role); }
   PRMroleChecker(false, "admin", "CriticalComponent", &RoleCheck, &Error, &Users, &Roles, &Components);
   for(const auto& Role : RoleCheck.m_Role) { UNIT_ASSERT(Role); }
}

void testPermissions(UNITapp* pApp) {
   pApp->add("permission/admin", &testAdminPermissions);
   pApp->add("permission/unsecure", &testUnsecure);
   pApp->add("permission/critical", &testCritical);
   pApp->add("permission/support", &testSupport);
}