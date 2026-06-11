// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testGithub
//
// Description:
//
// Implementation of tests
// 
// login credentials:
// 
// github-user         : iguana-interfaceware
// github-organization : iguana-interfaceware-org
// gmail               : iguana.interfaceware@gmail.com 
// password            : IguanaXpassword123
// token               : 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
// recovery codes      : f5950-3fc8d14a38-95929de5a9-2e3c5b628e-443a8a9c02-14b2e2a8a5-b12e42852c-73405abe13-3712bbd5b3-666ba82f48-2aeb818ab2-5545149b6f-ccd0c2df5e-7f0d3b671a-283d7de82a-e7eb5f41b9-b25a2
// 
// Author: Eliot Muir 
// Date:   Tuesday 28 March 2023 - 11:37AM
// ---------------------------------------------------------------------------
#include "testGithub.h"

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <GITC/GITCsshValid.h>
#include <GITE/GITEgithub.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

// Replace with your credentials
static GSVconfig auth(){
	GSVconfig Auth;
   Auth.m_Username = "iguana-interfaceware";    
   Auth.m_Token    = "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";
	Auth.m_ServerUrl = GSVdefaultHost(GIT_github_cloud);
   Auth.m_Local = false;
   return Auth;
}

static GSVconfig authBad(){
   GSVconfig Auth = auth();
   Auth.m_Token = Auth.m_Token + "-junk";
   return Auth;
}

static void testCreatePersonalRepo(){
   COL_FUNCTION(testCreatePersonalRepo);
   GITEgithubRepoNewPersonal(auth(), "test1231");
}
static void testCreatePersonalRepoBad(){
   COL_FUNCTION(testCreatePersonalRepoBad);
   bool success = true;
   try {
      GITEgithubRepoNewPersonal(authBad(), "test1231");
   }catch (COLerror& error) {
      success = false;
   }
   UNIT_ASSERT(!success);
}

static void testCreateOrganizationalRepo(){
   COL_FUNCTION(testCreateOrganizationalRepo);
   GITEgithubRepoNewOrganization(auth(), "iguana-interfaceware-org", "test1231");
}

static void testToken(){
   COL_FUNCTION(testToken);
   int ReturnCode = 0;
   COLstring Error;
   GITEgithubCheckToken("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", auth(), ReturnCode, &Error);
   COL_VAR2(ReturnCode, Error);
}

static void testSshAccess(){
   COL_FUNCTION(testSshAccess);
   COLstring Error;
   bool Success = GITCsshCheckAccess("admin", auth(), GSVitem(true, GIT_github_cloud),&Error);
   COL_VAR2(Success, Error);
}

static void testRegisterKey(){
   COL_FUNCTION(testRegisterKey);
   COLstring Error;
   bool Success = GITEgithubRegisterSshKey("ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAILpQzD/YENX0E0zrD8vDYHWezSf6C1z6sAJ/kfyOi9x6 created by IguanaX", auth(), &Error);
   COL_VAR2(Success, Error);
}

static void testOwners(){
   COL_FUNCTION(testOwners);
   COLstring Error;
   COLvar Result;
   GITEgithubUpdateOwners("admin", false, auth(), &Result, &Error);
   COL_VAR2(Result, Error);
}

static void testCheckRepo(){
   COL_FUNCTION(testCheckRepo);
   COLstring Error;
   COLvar Result;
   // GITEgithubCheckRepo("vismay-shah-ifw", "test-collection", auth(), &Result, &Error);
   // GITEgithubCheckRepo("GhubIfOrgTest", "Repo1", auth(), &Result, &Error);
   COL_VAR2(Result, Error);
   GITEgithubCheckRepo("hsdlfasd", "asdla", auth(), &Result, &Error);
   // COL_VAR2(Result, Error);
}

static void testCreate(){
   COL_FUNCTION(testCreate);
   try{
      // GITEgithubRepoNew(auth(), "vismay-shah-ifw", "autorepo");
      GITEgithubRepoNew(auth(), "GhubIfOrgTest", "autorepo");
      // GITEgithubRepoNew(auth(), "hsdlfasd", "asdla");
      COL_VAR("Success");
   } catch(const COLerror& Error) {
      COL_VAR("Failed");
      COL_VAR(Error.description());
   }
}

void testGithub(UNITapp* pApp) {
   // pApp->add("github/token", &testToken);
   // pApp->add("github/access", &testSshAccess);
   // pApp->add("github/registerkey", &testRegisterKey);
   // pApp->add("github/owners", &testOwners);
   // pApp->add("github/checkrepo", &testCheckRepo);
   // pApp->add("github/create", &testCreate);
   
   // pApp->add("github/repo/create/personal",     &testCreatePersonalRepo);
   // pApp->add("github/repo/create/personal/bad", &testCreatePersonalRepoBad);
   // pApp->add("github/repo/create/organization", &testCreateOrganizationalRepo);
}
