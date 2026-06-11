// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHLPindex
//
// Description:
//
// Implementation
//
// Author: Orkhan Amikishiyev 
// Date:   Tuesday 15 November 2022 - 11:56AM
// ---------------------------------------------------------------------------
#include <CACHE/CACHEfile.h>
#include <CFG/CFGconfigApi.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <HELP/HELPenumerate.h>
#include <HELP/HELPresolve.h>
#include <HELP/HELPutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void HELPlistDisc(const COLstring& Directory, COLvar* pResult, COLstring* pName) {
   COL_FUNCTION(HELPlistDisc);
   COL_VAR(Directory);
   COLhashmap<COLstring, COLstring> HelpMap;
   if(!Directory.is_null() && FILfileExists(Directory) && FILisDirectory(Directory)) {
      COL_TRC("Iterating help directory " << Directory << newline);
      HELPenumerateHelp(&HelpMap, Directory, "", pName);
   }
   for(const auto& it : HelpMap) {
      COLvar Item;
      Item["name"]     = it->first;
      Item["location"] = it->second;
      (*pResult)["list"].push_back(Item);
   }
   (*pResult)["name"] = *pName;
}

// help/index
void HELPlist(const COLwebRequest& Request) {
   COL_FUNCTION(HELPlist);
   const COLstring Component = COLvarRequiredString(Request.Data, "component");
   const bool      IsDev     = COLvarRequiredBool(Request.Data, "development");
   const ROLid     Id        = IsDev ? EDIT_SCRIPT : VIEW_SCRIPT;
   COLstring       Name      = COLvarGetString(Request.Data, "name", "");
   COLstring       Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, Id, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   COLvar Result;
   Result["list"].setArrayType();
   if(CACHEexists()) {
      CACHEgetHelpList(&Result["list"]);
      HELPreplaceMetatablePrefix(&Name);
   } else {
      HELPlistDisc(HELPglobalDir(), &Result, &Name);
   }
   const COLstring ComponentDir = CFGresolveRepoFile(Component, "", IsDev);
   HELPlistDisc(ComponentDir, &Result, &Name);  // get component help files
   COL_VAR(Result.json(1));
   return COLrespondSuccess(Request.Address, Result);
}