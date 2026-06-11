// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testClone
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testClone.h"

#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <GITU/GITUcache.h>
#include <GIT/GITcommits.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUtemplate.h>
#include <SCK/SCKloop.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void getTestCollection(GITUlist* pList){
   GITUrepoInfo GitRepo;
   GitRepo.GitUrl = "git@bitbucket.org:interfaceware/hl7components.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "git@bitbucket.org:interfaceware/iguana_tools.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "git@bitbucket.org:interfaceware/interfaceware-core.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "git@bitbucket.org:interfaceware/notifiers.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "git@bitbucket.org:interfaceware/tylers-collection.git";
   pList->List.push_back(GitRepo);
}

static void testCompleted(const bool Success, COLstring Error, SCKloop* pLoop, const bool Expected){
   COL_FUNCTION(testCompleted);
   COL_VAR2(Success, Error);
   pLoop->shutdown();
   UNIT_ASSERT_EQUALS(Expected, Success);
}

static void fetchGitRepo(const COLstring& Repo, const bool Expected){
   COL_FUNCTION(fetchGitRepo);
   GITUlist List;
   GITUrepoInfo GitRepo;
   GitRepo.GitUrl = Repo;
   List.List.push_back(GitRepo);
   COLthreadPool Pool;
   SCKloop Loop(&Pool);
   Pool.start();
   GITUcloneCollection("", &Loop, List, COLnewClosure2(&testCompleted, &Loop, Expected));
   Loop.start();
}
static void testBadUrl() {
   const COLstring GitUrl = "git@bitbucket.org:interfaceware/hl7_simulator/src/main";
   fetchGitRepo(GitUrl, false);
}

static void testCommits(){
   COL_FUNCTION(testCommits);
   COLstring ComponentId = "HL7_GitCommits_123";
   COLstring GitUrl = "git@bitbucket.org:interfaceware/hl7_simulator.git";
   fetchGitRepo(GitUrl, true);  // make sure we can run this test independently.
   COLstring Error;
   COLstring Output;
   COL_TRC("Creating template");
   GITUtemplateCreate(ComponentId, GitUrl, "", &Error);
   COLstring ComponentDir = DIRcomponentDevDir(ComponentId);
   COLstring LogString;
   if(!GITcommits(ComponentDir, &LogString, &Error, true, true)){
      COL_TRC("Error gertting git logs:" << Error);
      return;
   }
   COLvar Data;
   GITparseCommits(LogString, &Data);
   COL_VAR(Data);
}

static void testModule2(){
   COL_FUNCTION(testModule2);
   // TODO - issue when trying to clone two of the same at the same time
   const COLstring GitUrl = "git@bitbucket.org:interfaceware/hl7_simulator.git";
   fetchGitRepo(GitUrl, true);  // make sure we can run this test independently.
   COLstring Error;
   COLstring Output;
   COL_TRC("Creating template");
   GITUtemplateCreate("HL7_ggg123", GitUrl, "", &Error);
   COL_TRC("Done!");
}

static void testModule(){
   COL_FUNCTION(testModule);
   // COLstring GitUrl = "git@bitbucket.org:interfaceware/hl7_simulator.git";
   COLstring GitUrl = "git@bitbucket.org:tyler-brown/12341234file-reader.git";
   // fetchGitRepo(GitUrl);  // make sure we can run this test independently.
   COLstring Error;
   COLstring Output;
   COL_TRC("Creating template");
   GITUtemplateCreate("Tyler", GitUrl, "", &Error);
   COL_TRC("Done!");
}

// TODO - fix it trying to git clone /Users/tylerbrown/main2/GITexample/git_cache///
static void testSimple() {
   COL_FUNCTION(testSimple);
   GITUlist List;
   getTestCollection(&List);
   COLthreadPool Pool(1,5);
   SCKloop Loop(&Pool);
   Pool.start();
   GITUcloneCollection("", &Loop, List, COLnewClosure2(&testCompleted, &Loop, true));
   Loop.start();
}


void testClone(UNITapp* pApp){
   COL_FUNCTION(testSimple);
   // pApp->add("git/simple", &testSimple);
   // pApp->add("git/module", &testModule);
   // pApp->add("git/module_2", &testModule2);
   // pApp->add("git/repository/commits", &testCommits);
   pApp->add("git/bad_url", &testBadUrl);
}
