// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUscratchRepo
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 27 March 2023 - 10:06AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <GIT/GITinit.h>
#include <GITU/GITUscratchRepo.h>
COL_LOG_MODULE;

// Used as part of checking whether we have write permission for a repo.
void GITUscratchRepo(const COLstring& Dir, const COLstring& Username){
   COL_FUNCTION(GITUscratchRepo);
   if (FILfileExists(Dir)){
      return;
   }
   FILmakeFullDir(Dir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   COLstring Err;  // TODO error reporting :-(
   GITinit(Username, Dir, true, &Err);
}

void GITUscratchRepoRemove(const COLstring& Dir){
   COL_FUNCTION(GITUscratchRepoRemove);
   COLstring Error;
   if (!FILremoveFullDirNewSafe(Dir, &Error)){ // swallowing the error.
      COL_TRC("Error deleting scratch repository");
   }
}