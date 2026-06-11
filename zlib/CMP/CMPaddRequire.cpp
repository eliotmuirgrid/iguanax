//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPaddRequire.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   08/11/23 11:55 AM
//  ---------------------------------------------------------------------------
#include <CMP/CMPaddRequire.h>
#include <CMP/CMPutils.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

void CMPaddRequireImpl(const COLstring& Component, const COLstring& InputPath, bool Development, const COLaddress& Address) {
   COL_FUNCTION(CMPaddRequireImpl);
   COL_VAR2(Component, InputPath);
   COLstring Path = InputPath;
   COLstring MainPath;
   COLvar    Result;
   if(!CMPcomponentFileExists(Component, "main.lua", Development, &MainPath, &Result) || !FILfileExists(MainPath)) {
      COL_TRC("main.lua does not exist");
      return COLrespondError(Address, "Could not find main.lua");
   }
   COLstring FileContent;
   try {
      FILreadFile(MainPath, &FileContent);
      if(FILpathExtLast(Path, true) != ".lua") { return COLrespondError(Address, Path + " is not a lua file"); }
      Path = Path.substr(0, Path.size() - 4);
      Path.replace(COLstring(FIL_PATH_SEP_NATIVE), ".");
      FileContent.prepend("require \"" + Path + "\"\n");
      FILwriteFile(MainPath, FileContent);
   } catch(const COLerror& E) { return COLrespondError(Address, E.description()); }
   return COLrespondSuccess(Address);
}

// /component/add/require
void CMPaddRequire(const COLwebRequest& Request) {
   COL_FUNCTION(CMPaddRequire);
   const COLvar& Data = Request.Data;
   COLstring ComponentId, Error;
   COLstring Path      = COLvarRequiredString(Data, "path");
   bool Development    = COLvarRequiredBool  (Data, "development");
   if(!Development){
      return COLrespondError(Request.Address, "Cannot modify files in a run repository.");
   }
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   Path = FILpathSepToNative(Path);    // / and \ work on Windows and POSIX
   CMPaddRequireImpl(ComponentId, Path, Development, Request.Address);
}
