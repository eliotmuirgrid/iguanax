//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testApp
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 18 Nov 2014 10:23:15 EST
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <USR/USRuser.h>
#include <USR/USRmap.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>

COL_LOG_MODULE;

static COLmap<COLstring, USRuser> createTestUserMap(){
   COL_FUNCTION(createTestUserMap);
   COLmap<COLstring, USRuser> UserMap;
   COLarray<COLstring> Tags;
   // Create admin user
   Tags.push_back("#adminTestRole");
   USRuser Admin("admin", "password", Tags);
   UserMap.add("admin", Admin);
   // Create another local user
   Tags.clear();
   Tags.push_back("#foo");
   Tags.push_back("#bar");
   USRuser Local("local", "test123", Tags);
   UserMap.add("local", Local);
   // Create an external user
   Tags.clear();
   Tags.push_back("#remote");
   USRuser External("ldapUser", "ldapPass", Tags, true);
   UserMap.add("ldapUser", External);
   return UserMap;
}

static void testEncryption() {
   COL_FUNCTION(testEncryption);
   USRuser testUser;
   testUser.setPassword("password");
   UNIT_ASSERT_EQUALS(true, testUser.authenticate("password"));
}

static void testSerialization(){
   COL_FUNCTION(testSerialization);
   COLmap<COLstring, USRuser> UserMap = createTestUserMap();
   COL_TRC("Serializing all users without passwords");
   COLvar Result = USRmapToVarUsers(&UserMap, false);
   COL_VAR(Result);
   UNIT_ASSERT_EQUALS(3, Result.size());
   UNIT_ASSERT(Result.exists("admin"));
   UNIT_ASSERT(Result.exists("local"));
   UNIT_ASSERT(Result.exists("ldapUser"));
   UNIT_ASSERT(!Result["admin"].exists("password"));

   COL_TRC("Serializing all users with passwords");
   Result = USRmapToVarUsers(&UserMap, true);
   UNIT_ASSERT_EQUALS(3, Result.size());
   UNIT_ASSERT(Result.exists("admin"));
   UNIT_ASSERT(Result.exists("local"));
   UNIT_ASSERT(Result.exists("ldapUser"));
   UNIT_ASSERT(Result["admin"].exists("password"));
}

static void testToVar(){
   COL_FUNCTION(testToVar);
   COLmap<COLstring, USRuser> UserMap = createTestUserMap();
   COLvar Result = USRmapToVarUsers(&UserMap, true);
   USRuser testUser;
   testUser.setPassword("password");
   UNIT_ASSERT_EQUALS("admin",    Result["admin"]["username"]);
   UNIT_ASSERT_EQUALS("#adminTestRole", Result["admin"]["roles"][0]);
   UNIT_ASSERT       (testUser.authenticate("password"));
   UNIT_ASSERT       (!Result["admin"]["external"]);

   testUser.setPassword("test123");
   UNIT_ASSERT_EQUALS("local",    Result["local"]["username"]);
   UNIT_ASSERT_EQUALS("#foo",     Result["local"]["roles"][0]);
   UNIT_ASSERT_EQUALS("#bar",     Result["local"]["roles"][1]);
   UNIT_ASSERT       (testUser.authenticate("test123"));
   UNIT_ASSERT       (!Result["local"]["external"]);

   testUser.setPassword("ldapPass");
   UNIT_ASSERT_EQUALS("ldapUser", Result["ldapUser"]["username"]);
   UNIT_ASSERT_EQUALS("#remote",  Result["ldapUser"]["roles"][0]);
   UNIT_ASSERT       (testUser.authenticate("ldapPass"));
   UNIT_ASSERT       (Result["ldapUser"]["external"]);
}

void testApp(UNITapp* pApp) {
   pApp->add("app/encryption",  &testEncryption);
   pApp->add("users/serialize", &testSerialization);
   pApp->add("users/to_var",    &testToVar);
}

