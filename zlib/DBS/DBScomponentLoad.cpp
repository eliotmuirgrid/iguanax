// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBScomponentLoad
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
#include <DBS/DBScomponentLoad.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKworker.h>
COL_LOG_MODULE;

static void DBScomponentLoadCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(DBScomponentLoadCallback);
   COL_VAR(Result.json(1));
   COLrespondSuccess(Address, Result);
}

static bool DBSresolveFileLoad(const COLstring& Component, const COLstring& Path, const bool& IsDev, COLstring& FilePath, COLvar* pResult){
   COL_FUNCTION(DBSresolveFileLoad);
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

// TODO this code is prone to having some weird issues if there is one bad vmd...
static void DBScomponentLoadImpl(const COLvar& In, COLvar* pResult) {
   COL_FUNCTION(DBScomponentLoadImpl);
   const COLstring& ComponentId = In["component"];
   const COLvar&    PathArray   = In["path"];
   const bool       IsDev       = In["development"];
   COL_VAR3(PathArray, IsDev, ComponentId);
   COLstring FilePath, FileContents;
   for (int i = 0; i < PathArray.size(); i++) {
      const COLstring& Path = PathArray[i];
      COL_VAR2(i,Path);
      if(!DBSresolveFileLoad(ComponentId, Path, IsDev, FilePath, pResult)) { continue; }
      if     (!FILfileExists(FilePath)) { COL_TRC(FilePath << " does not exist."); continue; }
      else if(FILisDirectory(FilePath)) { COL_TRC(FilePath << " is a directory."); continue; } 
      COL_VAR(FilePath);
      FILreadFile(FilePath, &FileContents);
      if (FileContents.size() == 0){
         COL_TRC("Blank Schema file.");
         (*pResult)[Path].setMapType();
         COLvar Empty;
         Empty.setMapType();
         (*pResult)[Path]["tables"] = Empty;
         continue;
      }
      COL_VAR(FileContents);
      COL_TRC("About to do a not smart :-) load,");
      DBcollection Collection;
      COLstring Error;
      bool LegacyWarning; // Called in context of importer/tables GUI - no need to show legacy warning
      if(!DBsmartLoad(FileContents, &Collection, &Error, &LegacyWarning)) {
         (*pResult)[Path]["error"] = Error;
         continue;
      }
      COL_TRC("About to regenerate JSON");
      COLstring Output;
      DBsave(Collection, &Output);
      COL_VAR(Output);
      COLvar Data;
      Data.parse(Output);
      (*pResult)[Path] = Data;
   }
   COL_VAR(pResult->json(1));
}

// /component/db/load
void DBScomponentLoad(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(DBScomponentLoad);
   COL_VAR(Request.Data);
   const COLstring Component   = COLvarRequiredString(Request.Data, "component");
   const bool      Development = COLvarRequiredBool(Request.Data, "development");
   const COLvar    PathArray   = COLvarRequiredObject(Request.Data, "path");
   const ROLid     Id          = Development ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   CFGconfig*      pConfig     = CFGmapNode(pMap, Component);
   if(!pConfig) { return COLrespondError(Request.Address, Component + " does not exist"); }
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   SCKinvokeWorker(pLoop, DBScomponentLoadImpl, Request.Data, COLnewClosure1(DBScomponentLoadCallback, Request.Address));
}