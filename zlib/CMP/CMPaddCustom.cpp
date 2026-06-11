// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPaddCustom
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 29 October 2025 - 04:12PM
// ---------------------------------------------------------------------------

#include <CMP/CMPaddCustom.h>
#include <CMP/CMPutils.h>
#include <CFG/CFGmap.h>
#include <COL/COLvar.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <COL/COLvarUtils.h>
COL_LOG_MODULE;

void CMPaddCustomImpl(const COLstring& Component, const COLstring& Content, const bool& Development, const COLaddress& Address) {
   COL_FUNCTION(CMPaddCustomImpl);
   COL_VAR2(Component, Content);
   COLstring MainPath;
   COLvar Result;
   if(!CMPcomponentFileExists(Component, "main.lua", Development, &MainPath, &Result) || !FILfileExists(MainPath)) {
      COL_TRC("main.lua does not exist");
      return COLrespondError(Address, "Could not find main.lua");
   }
   try {
      COLstring FileContent;
      FILreadFile(MainPath, &FileContent);
      FileContent.prepend(Content + "\n\n");
      FILwriteFile(MainPath, FileContent);
   } catch(const COLerror& E) { return COLrespondError(Address, E.description()); }
   return COLrespondSuccess(Address);
}

void CMPaddCustom(const COLwebRequest& Request) {
   COL_FUNCTION(CMPaddCustom);
   const COLvar& Data = Request.Data;
   COLstring ComponentId, Error;
   COLstring Content = COLvarRequiredString(Data, "content");
   bool Development = COLvarRequiredBool(Data, "development");
   if(!Development){
      return COLrespondError(Request.Address, "Cannot modify files in a run repository.");
   }
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   CMPaddCustomImpl(ComponentId, Content, Development, Request.Address);
}