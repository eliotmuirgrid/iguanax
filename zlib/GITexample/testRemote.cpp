// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testRemote
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 05:08PM
// ---------------------------------------------------------------------------
#include "testRemote.h"

#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUaddRemote.h>
#include <GITU/GITUpermission.h>
#include <GITU/GITUremoveRemote.h>
#include <GITU/GITUscratchRepo.h>
#include <GITU/GITUurl.h>
#include <INI/INIcompare.h>
#include <PRO/PROexecute.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

// See IX-1603
static void testClearAndSet(){
   COL_FUNCTION(testClearAndSet);
   COLstring Input = "https://bitbucket.org/interfaceware/writeable";
   COLstring RepoDir = "writeable";
   COLstring RemoveError;
   if(FILfileExists("interfaceware/writeable") && !FILremoveFullDirNewSafe("interfaceware/writeable", &RemoveError)){
      COL_ERR("Failed to clean up from old test. " + RemoveError);
      UNIT_ASSERT(false);
   }
   if(FILfileExists(RepoDir) && !FILremoveFullDirNewSafe(RepoDir, &RemoveError)){
      COL_ERR("Failed to clean up from old test. " + RemoveError);
      UNIT_ASSERT(false);
   }
   COLstring SshUrl;
   bool Success = GITUsshUrl(Input, &SshUrl);
   COL_VAR(SshUrl);
   
   COLstring Out, Err;
   int ExitCode = PROexecuteString("", "git clone " + SshUrl, &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   FILcopyFile(RepoDir + FIL_DIR_SEPARATOR + ".git" + FIL_DIR_SEPARATOR + "config", "git_config_original.txt");
   COL_TRC("Saving the original .git/config as git_config_original.txt in a sorted format for easy comparison.");
   INIsortFile("git_config_original.txt");  
   GITUremoveRemote(RepoDir);
   FILcopyFile(RepoDir + FIL_DIR_SEPARATOR + ".git" + FIL_DIR_SEPARATOR + "config", "git_config_afterremove.txt");
   COL_TRC("Saving .git/config as git_config_afterremove.txt after removing remote");
   Out.clear();
   Err.clear();
   ExitCode = PROexecuteString(RepoDir, "git push --dry-run", &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   UNIT_ASSERT(ExitCode !=0);
   UNIT_ASSERT(Err.find("No configured push destination") != npos);
   GITUaddRemote(RepoDir, SshUrl, &Err);
   FILcopyFile(RepoDir + FIL_DIR_SEPARATOR + ".git" + FIL_DIR_SEPARATOR + "config", "git_config_afteradd.txt");
   COL_TRC("Saving .git/config as git_config_afteradd.txt after adding remote");
   Err.clear();
   Out.clear();
   ExitCode = PROexecuteString(RepoDir, "git push --dry-run", &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);
   UNIT_ASSERT(ExitCode ==0);
   UNIT_ASSERT_EQUALS("", Out);
   UNIT_ASSERT(Err.find("Everything up-to-date") != npos);

   COLstring OriginalConfig, FinalConfig;
   FILreadFile("git_config_original.txt", &OriginalConfig);
   FILreadFile("git_config_afteradd.txt", &FinalConfig);
   UNIT_ASSERT_EQUALS(OriginalConfig, FinalConfig);
}

static void testWritable(){
   COL_FUNCTION(testWritable);
   COLstring GitPermissionError;
   int ExitCode = -1;
   bool Writeable = GITUpermissionWrite("", "https://bitbucket.org/interfaceware/writeable.git", FILpathAppend(FILexecutableDir(), "testWriteable"),     &ExitCode, &GitPermissionError);
   bool Unwritable = GITUpermissionWrite("", "https://bitbucket.org/interfaceware/unwritable.git", FILpathAppend(FILexecutableDir(), "testUnwriteable"), &ExitCode, &GitPermissionError);
   UNIT_ASSERT_EQUALS(true, Writeable);
   UNIT_ASSERT_EQUALS(false, Unwritable);
   COL_VAR2(Writeable, Unwritable);
}

static void testCreateScratch(){
   COL_FUNCTION(testCreateScratch);
   COLstring Dir = "scratchgit";
   COLstring RemoveError;
   if (FILfileExists(Dir) && !FILremoveFullDirNewSafe(Dir, &RemoveError)){
      COL_ERR(RemoveError);
      UNIT_ASSERT(false);
   } else {
      COL_TRC("Test cleared up old repo already.");
   }
   GITUscratchRepo(Dir);
   UNIT_ASSERT_EQUALS(FILfileExists(Dir), true);
   GITUscratchRepoRemove(Dir);
   UNIT_ASSERT_EQUALS(FILfileExists(Dir), false);
}

void testRemote(UNITapp* pApp) {
   pApp->add("git/remote/clearandset", &testClearAndSet);
//   pApp->add("git/remote/writable",    &testWritable);
   pApp->add("git/create/scratch",     &testCreateScratch);
}
