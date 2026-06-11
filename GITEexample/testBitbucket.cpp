// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testBitbucket
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Tuesday 28 March 2023 - 11:36AM
// ---------------------------------------------------------------------------
#include "testBitbucket.h"

#include <COL/COLlog.h>
#include <GITE/GITEbitbucket.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static GSVconfig auth(){
   GSVconfig Auth;
   Auth.m_Username = "eliotmuir";    
   Auth.m_Token    = "ATBBq74M8safNzq3BAakz3yew7UQC52CF9CC";
	Auth.m_ServerUrl= GSVdefaultHost(GIT_bitbucket_cloud);
   return Auth;
}

typedef void (&TFunc)();

static void errorHelper(TFunc pFunc, const COLstring& ExpectedError){
   COL_FUNCTION(errorHelper);
   COLstring Message;
   try{
      (*pFunc)();
   } catch (const COLerror& Error){
      Message = Error.description();
   }
   UNIT_ASSERT_EQUALS(ExpectedError, Message);
}

static void testUser(){
   COL_FUNCTION(testUser);
   GITEbitbucketUser(auth());
}

static void testCreate(){
   COL_FUNCTION(testCreate);
   try{
      GITEbitbucketRepoDelete(auth(), "interfaceware", "test123");  // yikes - this is dangerous! 
   } catch(...){
      // clean up previous bad test.
   }
   GITEbitbucketRepoNew   (auth(), "interfaceware", "test123");
   COLmap<COLstring, COLstring> List;
   GITEbitbucketRepoList(auth(), "interfaceware", &List);
   // UNIT_ASSERT(List.count("test123") == 1);  // TODO - the list isn't complete yet!

   GITEbitbucketRepoDelete(auth(), "interfaceware", "test123");  // yikes - this is dangerous! 
   GITEbitbucketRepoList(auth(), "interfaceware", &List);
   UNIT_ASSERT(List.count("test123") == 0);
}

static void testCreateBadImp(){
   COL_FUNCTION(testCreateBadImp);
   GITEbitbucketRepoNew(auth(), "nonexistentworkspace", "test123"); 
}

static void testCreateBad(){ errorHelper(testCreateBadImp, "No workspace with identifier 'nonexistentworkspace'.");}

static void testDeleteBadImp(){
   COL_FUNCTION(testDeleteBadImp);
   GITEbitbucketRepoDelete(auth(), "interfaceware", "nonexistingrepositoryfortest");  // yikes - this is dangerous!
}

static void testDeleteBad(){
   COL_FUNCTION(testDeleteBad);
   errorHelper(testDeleteBadImp, "You may not have access to this repository or it no longer exists in this workspace. If you think this repository exists and you have access, make sure you are authenticated.");
}

static void testList(){
   COL_FUNCTION(testList);
   COLmap<COLstring, COLstring> List;
   GITEbitbucketRepoList(auth(), "interfaceware", &List);
   UNIT_ASSERT(List.count("main2") == 1);
   COL_VAR(List.size());
}

static void testListBadImp(){
   COLmap<COLstring, COLstring> List;
   GITEbitbucketRepoList(auth(), "badworkspace", &List);
}

static void testListBad(){
   COL_FUNCTION(testListBad);
   errorHelper(testListBadImp, "No workspace with identifier 'badworkspace'.");
}

// Having a unit test which generates a key and registers it would result in
// an intolerable amount of noise after a while.  However it would be good to check
// that we get the right error
static void testRegisterKeyBad(){
   COL_FUNCTION(testRegisterKeyBad);
   COLstring Error;
   GITEbitbucketRegisterSshKey("A bad key!", auth(), &Error);
   UNIT_ASSERT_EQUALS("Key or authorization token invalid.", Error);
}


static void testRegisterKeyTwice(){
   COL_FUNCTION(testRegisterKeyTwice);
   COLstring Error;
   GITEbitbucketRegisterSshKey("ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIAoxEfjXzcwMC+IVpPoLywY20M8BdjHj4wTDRJy+BozM created by IguanaX", auth(), &Error);
   UNIT_ASSERT_EQUALS("Someone has already added that SSH key.", Error);
}

void testBitbucket(UNITapp* pApp) {
   pApp->add("bitbucket/user"      , &testUser);
   pApp->add("bitbucket/create"    , &testCreate);
   pApp->add("bitbucket/create/bad", &testCreateBad);
   pApp->add("bitbucket/delete/bad", &testDeleteBad);
   pApp->add("bitbucket/list",       &testList);
   pApp->add("bitbucket/list/bad",   &testListBad);  
   pApp->add("bitbucket/registerkey/bad", &testRegisterKeyBad);
   pApp->add("bitbucket/registerkey/twice", &testRegisterKeyTwice);
}
