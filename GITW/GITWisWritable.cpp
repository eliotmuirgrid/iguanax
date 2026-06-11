// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITisWritable
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 27 March 2023 - 09:42AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GIT/GITpath.h>
#include <GITU/GITUpermission.h>
#include <GITW/GITWisWritable.h>
#include <GSV/GSVlink.h>
COL_LOG_MODULE;

// /git/writable
void GITWisWritable(const COLwebRequest& Request) {
   COL_FUNCTION(GITWisWritable);
   const COLstring GitUrl  = COLvarRequiredString(Request.Data, "repo");
   const COLstring Scratch = GITuserScratchDir(Request.User, GitUrl);
   COLvar          Result;
   COLstring       Error;
   int             ExitCode = -1;
   Result["writable"]       = GITUpermissionWrite(Request.User, GitUrl, Scratch, &ExitCode, &Error);
   GSVlink    Link;
   const bool Success = GSVconstructLink(GitUrl, &Link);
   COLvar     Links   = GSVlinkToVar(Link);
   if(Success) {
      Result["link"] = Links["web"];
   } else {
      Result["error"] = "Error creating link for " + GitUrl;
   }
   COL_VAR(Result.json(1));
   FILremoveFullDirNewSafe(Scratch, &Error);
   COLrespondSuccess(Request.Address, Result);
}
