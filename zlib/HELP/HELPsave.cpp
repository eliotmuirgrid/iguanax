//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPsave
//
// Description:
//
// Implementation
// 
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include <HELP/HELPsave.h>
#include <HELP/HELPutils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <CFG/CFGmap.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void HELPsaveDisc(const COLstring& Path, const COLstring& Component, const COLstring& Content,
                         const COLaddress& Address) {
   COL_FUNCTION(HELPsaveDisc);
   if (HELPinvalidComponent(Component))          { return COLrespondError(Address, "Could not find component."); }
   if (HELPcheckPathOutOfBounds(Path, Component)){ return COLrespondError(Address, "Help file not found within component directory or global help directory.");}
   if (!FILfileExists(Path))                     { return COLrespondError(Address, "There is no help file for this function."); }
   COLvar Data;
   HELPcontentToVar(Content, &Data);
   FILwriteFile(Path, Data.json(1));
   COL_VAR(Data.json(1));
   return COLrespondSuccess(Address, Data);
}

// /component/help/set
void HELPsave(const COLwebRequest& Request) {
   COL_FUNCTION(HELPsave);
   COLstring ComponentId, Error;
   const COLstring Path      = COLvarRequiredString(Request.Data, "path");
   const COLvar    Content   = COLvarRequiredObject(Request.Data, "content").json();
   if (!CFGcomponentId(Request.Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   COL_VAR3(Path, ComponentId, Content);
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   HELPsaveDisc(Path, ComponentId, Content, Request.Address);
}