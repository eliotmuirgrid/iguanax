// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBScomponentSet
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 08 November 2022 - 08:50AM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <CMP/CMPutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DB/DBgrammar.h>
#include <DB/DBgrammarLoad.h>
#include <DB/DBgrammarSave.h>
#include <DBS/DBScomponentSet.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void DBScomponentSetCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(DBScomponentSetCallback);
   Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"]);
}

static void DBScomponentSetError(const COLstring& Error, COLvar* pResult) {
   COL_FUNCTION(DBScomponentSetError);
   (*pResult)["success"] = false;
   (*pResult)["error"] = Error;
}

static bool DBSresolveFilesSet(const COLstring& Component, const COLstring& Path, const bool& IsDev, COLstring& FilePath, COLvar* pResult){
   COL_FUNCTION(DBSresolveFilesSet);
   COLstring ResolvedFilePath = FILexpand(Path);
   if (FILpathIsAbsolute(ResolvedFilePath)){
      COL_TRC("Provided path was an absolute path");
      FilePath = ResolvedFilePath;
      return FILfileExists(ResolvedFilePath);
   } else {
      COL_TRC("Try to resolve the path using the component directory");
      if     (!CMPcomponentFileExists(Component, Path, IsDev, &FilePath, pResult)) {  return false; }
      if     (!FILfileExists(FilePath)) { COL_TRC(FilePath << " does not exist.");   return false; }
      else if(FILisDirectory(FilePath)) { COL_TRC(FilePath << " is a directory.");   return false; } 
   }
   return true;
}

static void DBScomponentSetImpl(const COLvar& In, COLvar* pResult){
   COL_FUNCTION(DBScomponentSetImpl);
   const COLstring& Component = In["component"];
   const COLstring& Path      = In["path"];
   const COLstring& Content   = In["content"];
   const bool       IsDev     = In["development"];

   DBcollection Collection;
   COLstring FilePath, FileContents, Error;
   bool DummyLegacyWarning = false; // Called in context of importer/tables GUI - no need to show legacy warning
   if(!DBSresolveFilesSet(Component, Path, IsDev, FilePath, pResult))   { return DBScomponentSetError("A file with this name does not exist", pResult);}
   if(!FILfileExists(FilePath))                                            { return DBScomponentSetError("A file with this name does not exist", pResult); }
   else if(FILisDirectory(FilePath))                                       { return DBScomponentSetError("You are trying to write to a folder", pResult); }
   if(!DBsmartLoad(Content, &Collection, &Error, &DummyLegacyWarning))     { return DBScomponentSetError(Error, pResult); }
   DBsave  (Collection, &FileContents);
   FILwriteFile(FilePath, FileContents);
   (*pResult)["success"] = true;
}

// /component/db/set
void DBScomponentSet(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(DBScomponentSet);
   const COLstring Component   = COLvarRequiredString(Request.Data, "component");
   const COLstring Path        = COLvarRequiredString(Request.Data, "path");
   const COLstring Content     = COLvarRequiredString(Request.Data, "content");
   const bool      Development = COLvarRequiredBool(Request.Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   CFGconfig* pConfig = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, Component + " does not exist"); }
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_SCRIPT, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   SCKinvokeWorker(pLoop, DBScomponentSetImpl, Request.Data, COLnewClosure1(DBScomponentSetCallback, Request.Address));
}