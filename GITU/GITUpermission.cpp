// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUpermission
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 26 March 2023 - 05:21PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfigFlag.h>
#include <GITU/GITUpermission.h>
#include <GITU/GITUscratchRepo.h>
#include <GITU/GITUurl.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

// Modify to capture error code
bool GITUpermissionWrite(const COLstring& Username, const COLstring& RawUrl, const COLstring& ScratchDir, int* pExitCode, COLstring* pErr){
   COL_FUNCTION(GITUpermissionWrite);
   GITUscratchRepo(ScratchDir, Username);
   COLstring SshUrl;
   if (!GITUsshUrl(RawUrl, &SshUrl)){
      *pErr = "Failed to convert git url to ssh format";
      return false;
   }
   COL_VAR2(RawUrl, SshUrl);
   COLstring Err, Out;
   COLstring Command = "git" + GITUconfigFlag(Username) + " push --set-upstream " + SshUrl + " main --dry-run";
   COL_VAR(Command);
   int ExitCode = PROexecuteString(ScratchDir, Command, &Out, &Err, 25, false);
   COL_VAR3(ExitCode, Out, Err);
   *pExitCode = ExitCode;
   *pErr = Err;
   COL_TRC("We expect ExitCode==1 or ExitCode==0 for a repo that we have access to even though this is a blank repo");
   GITUscratchRepoRemove(ScratchDir);
   /*COLstring Out2;
   Out2 = "ExitCode = " + COLintToString(ExitCode) + "\n";
   Out2+= "Command=" + Command + "\n";
   Out2+= "Out=" + Out + "\n";
   Out2+= "Err=" + Err + "\n";
   FILappendFile("/Users/eliotmuir/main2/Iguana/git.log", Out2);*/

   return ExitCode==0 || ExitCode==1; 
}