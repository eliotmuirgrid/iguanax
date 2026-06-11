//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPload
//
// Description:
//
// Implementation
// 
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include <CACHE/CACHEfile.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <HELP/HELPload.h>
#include <HELP/HELPutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void HELPloadImpl(const COLstring& Path, const COLstring& Component, const COLaddress& Address) {
   COL_FUNCTION(HELPloadImpl);
   COL_VAR2(Path, Component);
   COLstring Data;
   if(HELPinvalidComponent(Component)) { return COLrespondError(Address, "Could not find component."); }
   if(!CACHEreadFile(Path, &Data))     { return COLrespondError(Address, Path + " not found"); }
   COLvar Result;
   if(!CACHEexists()) {
      COL_TRC("Got data from disk");
      if(HELPcheckPathOutOfBounds(Path, Component)) {
         return COLrespondError(Address, "Help file not found within component directory or global help directory.");
      }
   }
   HELPcontentToVar(Data, &Result);
   Result["is_core"] = Path.find(DIRcomponentDevDir(Component)) == npos; // we do not want to allow editing for core help files
   COL_VAR(Result);
   return COLrespondSuccess(Address, Result);
}

// /component/help
void HELPload(const COLwebRequest& Request) {
   COL_FUNCTION(HELPload);
   COLstring ComponentId, Error;
   const COLstring Path      = COLvarRequiredString(Request.Data, "path");
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   COL_VAR2(ComponentId, Path);
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::VIEW_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::VIEW_SCRIPT));
   }
   HELPloadImpl(Path, ComponentId, Request.Address);
}