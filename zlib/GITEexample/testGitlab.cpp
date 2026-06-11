//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testGitlab.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   29/03/24 7:09 PM
//  ---------------------------------------------------------------------------
#include "testGitlab.h"

#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <GITC/GITCsshValid.h>
#include <GITE/GITEgitlab.h>
#include <GITE/GITEutils.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
#include <SCK/SCKloop.h>
#include <SCKS/SCKSsslConnection.h>
#include <SCKS/SCKSsslContext.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

GSVconfig createCreds(){
	GSVconfig GoodCreds;
   GoodCreds.m_Username = "";
   GoodCreds.m_Token    = "glpat-WGDV1rm3VKy8qnhkqYyG";
	GoodCreds.m_ServerUrl = GSVdefaultHost(GIT_gitlab_cloud);
   return GoodCreds;
}

// set createAuth to your own credentials
static void testGitlabCreate() {
   COL_FUNCTION(testGitlabCreate);
   GSVconfig Auth = createCreds();
   // GITEgitlabRepoNew(Auth, "test", "unittest"); // commented out to prevent spamming repositories
}

// static void testToken(){
//    // replace 'your username' and 'your token'
//    UNIT_ASSERT(!GITEgitlabCheckToken("your username", "foo"));
//    UNIT_ASSERT(GITEgitlabCheckToken("your username", "your token"));
// }

// static void testAccess(){
//    // set up your credentials in createCreds and your private key in 'your key'
//    GITEconfig GoodCreds = createCreds();
//    COLstring PrivateKey = "your key";
//    COLstring Error;
//    bool success = false;
//    success = GITCsshCheckAccess(PrivateKey, GoodCreds, &Error);
//    COL_VAR2(success, Error);
//    UNIT_ASSERT(success);
// }

// General purpose api tester for GET requests. For POST, you'll need to make a similar GITEgitlabGeneralApiCall function which does a blocking post request
static void testApi(){
   COL_FUNCTION(testApi);
   // set up your credentials in createCreds
	GSVconfig GoodCreds = createCreds();
   if (GoodCreds.m_Token.is_null()) { COL_ERR("Set up credentials first"); }
   // COLstring Endpoint = "/groups?min_access_level=30"; // this returns the list of groups you have access to
   COLstring Endpoint = "/projects/56678022"; // try and get personal user group - is there a setting to check such as 'can_create_project'
   COLstring Error;
   COLvar Result;
	GITEgitlabGeneralApiCall(Endpoint, GoodCreds, &Result, &Error);
}

void testGitlab(UNITapp* pApp) {
//   pApp->add("gitlab/create_repo", &testGitlabCreate);
//   pApp->add("token", &testToken);
//   pApp->add("ssh", &testSsh);
//   pApp->add("access", &testAccess);
   // pApp->add("general", &testApi);
//   pApp->add("foo", &foo);
}
