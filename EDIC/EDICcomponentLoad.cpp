// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDICcomponentLoad
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 19 October 2022 - 01:49PM
// ---------------------------------------------------------------------------
#include <CMP/CMPutils.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h>
#include <EDI/EDIsave.h>
#include <EDIC/EDICcomponentLoad.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static bool EDICresolveFile(const COLstring& Component, const COLstring& Path, const bool& IsDev, COLstring& FilePath, COLvar* pResult){
   COL_FUNCTION(EDICresolveFile);
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

static bool EDICreadVmdFile(const COLstring& Component, const COLstring& Path, const bool IsDev, COLvar* pResult, COLstring* pFileContents){
   COLstring FilePath;
   if (!EDICresolveFile(Component, Path, IsDev, FilePath, pResult)) { return false; }
   COL_VAR(FilePath);
   try {
      FILreadFile(FilePath, pFileContents);
   } catch (const COLerror& Error){
      COL_TRC("Failed to read file: " << FilePath);
      (*pResult)[Path] = Error.description();
      return false;
   }
   if (pFileContents->size() == 0){
      COL_TRC("Blank vmd file.");
      (*pResult)[Path].setMapType();
      COLvar Nodes;
      Nodes.setMapType();
      (*pResult)[Path]["nodes"] = Nodes;
      COLvar Matching;
      Matching.setArrayType();
      (*pResult)[Path]["matching"] = Matching;
      return false;
   }
   return true;
}

static void EDICloadVmd(const COLstring& Component, const COLstring& Path, const bool IsDev, COLvar* pResult){
   COL_FUNCTION(EDICloadVmd);
   COLvar& Result = *pResult;
   COLstring FileContents;
   if (!EDICreadVmdFile(Component, Path, IsDev, &Result, &FileContents)) { return; }
   COL_TRC("About to do a smart load,");
   EDIcollection Collection;
   COLstring Error;
   bool LegacyWarning; // Called in context of importer/hl7 GUI - no need to show legacy warning
   COLstring ConversionWarning;
   if(!EDIsmartLoad(FileContents, &Collection, &Error, &ConversionWarning,  &LegacyWarning)) { 
      Result[Path]["error"] = Error;
      return;
   }
   COL_TRC("About to regenerate JSON");
   COLstring Output;
   if (!EDIsave(Collection, &Output, &Error)){
      Result[Path]["error"] = Error;
      return;
   }
   // COL_VAR(Output);
   COLvar Data;
   Data.parse(Output);
   Result[Path] = Data;
}

static void EDICcomponentLoadImpl(const COLstring& Component, const COLvar& PathArray, const bool IsDev, const COLaddress& Address) {
   COL_FUNCTION(EDICcomponentLoadImpl);
   COL_VAR3(PathArray, IsDev, Component);
   COLvar Result;
   for (int i = 0; i < PathArray.size(); i++) {
      EDICloadVmd(Component, PathArray[i], IsDev, &Result);
   }
   // COL_VAR(Result.json(1));
   COL_TRC("Done");
   COLrespondSuccess(Address, Result);
}

// /component/edi/load
void EDICcomponentLoad(const COLwebRequest& Request){
   COL_FUNCTION(EDICcomponentLoad);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   COLstring ComponentId, Error;
   bool IsDev = COLvarRequiredBool(Data, "development");
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   ROLid Id = IsDev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, Id, &Error)){
      return COLrespondError(Request.Address, PRMsetError(Id));
   }
   EDICcomponentLoadImpl(
      ComponentId,
      COLvarRequiredObject(Data, "path"),
      IsDev,
      Request.Address);
}
