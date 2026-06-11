// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTdeleteVersion
//
// Description: Implementation
//
// Author: Matthew Sobkowski
// Date:   Monday 28 August 2023 - 10:36AM
// ---------------------------------------------------------------------------
#include <INST/INSTdeleteVersion.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <COL/COLweb.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring INSTdownloadDir(){
   COL_FUNCTION(INSTdownloadDir);
   COLstring Dir = FILpathAppend(FILworkingDir(), "upgrades/");
   COL_VAR(Dir);
   return Dir;
}

void INSTdeleteVersion(const COLwebRequest& Request) {
   COL_FUNCTION(INSTdeleteVersion);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::UPGRADE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::UPGRADE));
   }
   COLstring VersionFile = COLvarRequiredString(Request.Data, "version_file");
   VersionFile = INSTdownloadDir() + VersionFile;
   if(!FILfileExists(VersionFile)) {
      COL_TRC(VersionFile + " does not exist");
      return COLrespondError(Request.Address, VersionFile + " does not exist");
   }
   FILremove(VersionFile, true);
   COLrespondSuccess(Request.Address);
}