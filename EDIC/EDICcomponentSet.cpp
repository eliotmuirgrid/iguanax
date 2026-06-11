//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation
//
// Author:   Eliot Muir
// Date:     26th July 2022
//---------------------------------------------------------------------------
#include <CMP/CMPutils.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h>
#include <EDI/EDIsave.h>
#include <EDIC/EDICcomponentSet.h>  // switching to using full file names since it makes it easier to move code around
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void EDICcomponentSetImpl(const COLstring& Component, const COLstring& Path, const COLstring& Content, const bool& IsDev, const COLaddress& Address){
   COL_FUNCTION(EDICcomponentSetImpl);   
   COLvar Result;
   EDIcollection Collection;
   COLstring FilePath, FileContents;
   if(!CMPcomponentFileExists(Component, Path, IsDev, &FilePath, &Result)) { return COLrespondError(Address, "A file with this name does not exist"); }
   if(!FILfileExists(FilePath))      { return COLrespondError(Address, "A file with this name does not exist"); }
   else if(FILisDirectory(FilePath)) { return COLrespondError(Address, "You are trying to write to a folder"); } 
   COLstring Error;
   bool LegacyWarningDummy = false; // Called in context of importer, no need to show legacy warning
   COLstring ConvWarningDummy;
   if(!EDIsmartLoad(Content, &Collection, &Error, &ConvWarningDummy, &LegacyWarningDummy)){ return COLrespondError(Address, Error); }
   if(!EDIsave(Collection, &FileContents, &Error)) { return COLrespondError(Address, Error); }
   FILwriteFile(FilePath, FileContents);
   COLrespondSuccess(Address, Result);
}

// https://interfaceware.atlassian.net/wiki/spaces/DEVELOPMEN/pages/2354872343/API+spec
// /component/edi/set
void EDICcomponentSet(const COLwebRequest& Request){
   COL_FUNCTION(EDIcomponentSet);   
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   COLstring ComponentId, Error;
   bool Development          = COLvarRequiredBool(Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   EDICcomponentSetImpl(
      ComponentId,
      COLvarRequiredString(Data, "path"),
      COLvarRequiredString(Data, "content"),
      COLvarRequiredBool  (Data, "development"),
      Request.Address);
}
