// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcreateRepo
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 21 November 2023 - 09:58AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITE/GITEcreateRepo.h>
#include <GITW/GITWcreateRepo.h>
COL_LOG_MODULE;

// /git/create/repo
void GITWcreateRepo(const COLwebRequest& Request) {
   COL_FUNCTION(GITWcreateRepo);
   COL_VAR(Request.Data.json(1));
   const COLstring Owner      = COLvarRequiredString(Request.Data, "owner");
   const COLstring Repository = COLvarRequiredString(Request.Data, "repository");
   COL_TRC("Checking owner string");
   if (Owner.is_null() || Owner.isWhitespace()) { 
      return COLrespondError(Request.Address, "Must provide owner");
   }
   COL_TRC("Checking repository string");
   if (Repository.is_null() || Repository.isWhitespace()) {
      return COLrespondError(Request.Address, "Must provide a name for the repository");
   }
   COL_TRC("Owner and repository are defined.");
	COLstring GitUrl, Error;
	bool Success = GITEcreateRepo(Request.User, Owner, Repository, &GitUrl, &Error);
	Success ? COLrespondSuccess(Request.Address, GitUrl) :  COLrespondError(Request.Address, Error);
}