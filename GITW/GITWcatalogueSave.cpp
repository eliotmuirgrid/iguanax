// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcatalogueSave
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Friday 19 May 2023 - 12:05PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUlistLoad.h>
#include <GITW/GITWcatalogueSave.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

// /git/catalogue/save
void GITWcatalogueSave(const COLwebRequest& Request){
   COL_FUNCTION(GITWcatalogueSave);
   COL_VAR(Request.Data);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_COLLECTIONS, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_COLLECTIONS));
   }
   const COLvar& Content = COLvarRequiredObject(Request.Data, "content");
   GITUlist List;
   GITUlistCatalogueMap(Content, Request.User, true, &List);
   GITUlistCatalogueSave(List);
   COLrespondSuccess(Request.Address);
}
