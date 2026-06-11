// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcatalogueAdd
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 27 November 2023 - 07:49AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUlistLoad.h>
#include <GITU/GITUurl.h>
#include <GITW/GITWcatalogueAdd.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void GITWcatalogueAddImpl(const COLstring& GitUrl, const COLaddress& Address){
   COL_FUNCTION(GITWcatalogueAddImpl);
   GITUlist List;
   if (!GITUlistCatalogueLoad(&List)){
      return COLrespondError(Address, "Failed to load git collection.");
   }
   COLstring SshGitUrl;
   if (!GITUsshUrl(GitUrl, &SshGitUrl)){
      return COLrespondError(Address, "Failed to convert git url to ssh format");
   }
   GITUrepoInfo GitRepo;
   GitRepo.GitUrl = SshGitUrl;
   List.List.push_back(GitRepo);
   GITUlistCatalogueSave(List);
   COLvar Result;
   // TODO - this needs to return the links COLvar
   Result["git"] = SshGitUrl;
   return COLrespondSuccess(Address, Result);
}

// /git/catalogue/add
void GITWcatalogueAdd(const COLwebRequest& Request){
   COL_FUNCTION(GITWcatalogueAdd);
   const COLvar& Data = Request.Data;
   COLstring GitUrl = COLvarRequiredString(Data, "git");
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_COLLECTIONS, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_COLLECTIONS));
   }
   GITWcatalogueAddImpl(GitUrl, Request.Address);
}