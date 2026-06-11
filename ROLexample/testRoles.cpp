//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testRoles.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   24/01/24 4:17 PM
//  ---------------------------------------------------------------------------
#include "testRoles.h"

#include <UNIT/UNITapp.h>

#include <FIL/FILutils.h>

#include <PASS/PASSword.h>

#include <ROL/ROLrole.h>
#include <ROL/ROLserialize.h>

#include <ROL/ROLaddRole.h>
#include <ROL/ROLdeleteRole.h>
#include <ROL/ROLeditRole.h>
#include <ROL/ROLtest.h>

#include <SCK/SCKloop.h>

#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void setupRoleMap(ROLmap* pRoleMap) {
   ROLadd(pRoleMap, "#test", ROLrole());
   ROLadd(pRoleMap, "#test2", ROLrole());
   ROLadd(pRoleMap, "#test3", ROLrole());
   ROLadd(pRoleMap, "#test4", ROLrole(true));
   ROLadd(pRoleMap, "#test5", ROLrole(true));
}

static void mapCallack(COLvar Result, COLstring Filename, SCKloop* pLoop) {
   COL_FUNCTION(mapCallack);
   COL_VAR(Result);
   pLoop->shutdown();
   if(FILfileExists(Filename)) { FILremove(Filename); }
   COLASSERT(Result["success"].asBool());
}

static void testEditTags() {
   COL_FUNCTION(testEditRole);
   ROLmap Map;
   setupRoleMap(&Map);
   COLarray<COLstring> HasEdit;
   HasEdit.push_back("#test");
   HasEdit.push_back("#test4");
   COLarray<COLstring> NoEdit;
   NoEdit.push_back("#test");
   COLarray<COLstring> DNE;
   NoEdit.push_back("#iDontExist");
   COLASSERT(ROLtestImpl(HasEdit, &Map));
   COLASSERT(!ROLtestImpl(NoEdit, &Map));
   COLASSERT(ROLtestImpl(DNE, &Map));
}

static void testEditRole() {
   COL_FUNCTION(testEditRole);
   PASSsetKeyIV("test", "test");
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   auto pRoleMap = new ROLmap();
   setupRoleMap(pRoleMap);
   const COLstring File = "editRoleTest";
   const COLstring RoleName = "#test";
   ROLrole Role(false);
   ROLeditRoleImpl("testUser", pRoleMap, RoleName, ROLroleToVar(Role), File, &Loop, COLnewClosure1(&mapCallack, File, &Loop));
   Loop.start();
   delete pRoleMap;
}

static void testDeleteRole() {
   COL_FUNCTION(testDeleteRole);
   PASSsetKeyIV("test", "test");
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   auto pRoleMap = new ROLmap();
   setupRoleMap(pRoleMap);
   const COLstring File = "deleteRoleTest";
   const COLstring RoleName = "#test2";
   ROLdeleteRoleImpl("testUser", pRoleMap, RoleName, File, &Loop, COLnewClosure1(&mapCallack, File, &Loop));
   Loop.start();
   delete pRoleMap;
}

static void testAddRole() {
   COL_FUNCTION(testAddRole);
   PASSsetKeyIV("test", "test");
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   auto pRoleMap = new ROLmap();
   setupRoleMap(pRoleMap);
   const COLstring File = "addRoleTest";
   const COLstring RoleName = "#newRole";
   ROLaddRoleImpl("testUser", pRoleMap, RoleName, File, &Loop, COLnewClosure1(&mapCallack, File, &Loop));
   Loop.start();
   delete pRoleMap;
}

static void testIguanaWide() {
   COL_FUNCTION(testIguanaWide);
   COLvar        InvalidRole;
   const ROLrole Role;
   const COLvar IguanaOnly = ROLroleToVarIguanaOnly(Role);
   COL_VAR(IguanaOnly.json(1));
   COLASSERT(IguanaOnly.size() == 24);
}

static void testRoleBasic() {
   COL_FUNCTION(testRoleBasic);
   auto pRoleMap = new ROLmap();
   setupRoleMap(pRoleMap);
   COLvar Roles = ROLmapToVar(*pRoleMap);
   COL_VAR(Roles.json(1));
   COLASSERT(Roles.size() == 5);
   delete pRoleMap;
}

void testRoles(UNITapp* pApp) {
   pApp->add("role/basic",           &testRoleBasic);
   pApp->add("role/iguana_wide",     &testIguanaWide);
   pApp->add("role/add",             &testAddRole);
   pApp->add("role/delete",          &testDeleteRole);
   pApp->add("role/edit",            &testEditRole);
   pApp->add("role/test_edit_tags",  &testEditTags);
}
