// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEutils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tuesday 09 April 2024 - 12:59PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GITE/GITEutils.h>
#include <SFI/SFIuriEscapeFilter.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static bool GITEextraLogging(){
   COL_FUNCTION(GITEextraLogging);
   bool ExtraLogging = false;
   if (getenv("IFW_AZURE_EXTRA_LOGGING")){ ExtraLogging = true; }
   // TODO - add the extra logging checks for each git flavour
   return ExtraLogging;
}

void GITEremoveFileIfForce(bool Force, const COLstring& FileName){
   COL_FUNCTION(GITEremoveFileIfForce);
   if(Force && FILfileExists(FileName)){
      COL_TRC("Removing file " + FileName);
      FILremoveWithThrow(FileName);
   }
}

bool GITEupdateOwnersFile(const COLstring& Username, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEupdateOwnersFile);
   COLstring FileName = DIRconfigUserOwnersFile(Username);
   bool ExtraLogging = GITEextraLogging();
   if(ExtraLogging) { SDBIlogService("Will update the file " + FileName); }
   try {
      FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      FILwriteFile(FileName, pResult->json(1));
      if(ExtraLogging) { SDBIlogService("Owners file updated successfully "); }
      return true;
   }
   catch (const COLerror& E) {
      COL_ERR(E.description());
      *pError = E.description();
      if(ExtraLogging) { SDBIlogService("Failed to update owners file. Error: " + E.description()); }
      return false;
   }
}

bool GITEreadOwnersFile(const COLstring& FileName, COLstring* pContent, COLstring* pError){
   COL_FUNCTION(GITEreadOwnersFile);
   try{
      FILreadFile(FileName, pContent);
      return true;
   } catch (const COLerror& Error){
      *pError = Error.description();
      return false;
   }
}

bool GITElistOwners(const COLstring& Username, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITElistOwners);
   COLvar& Result = *pResult;
   COLvar& Owners = Result["owners"];
   Owners.setArrayType();
   COLstring FileName = DIRconfigUserOwnersFile(Username);
   COL_VAR(FileName);
   bool Exists = FILfileExists(FileName);
   Result["exists"] = Exists;
   if (!Exists){
      COL_TRC("No owners are cached");
      return true;
   }
   COLstring Content;
   if (!GITEreadOwnersFile(FileName, &Content, pError)) { return false; }
   COLvar Data;
   COLstring Error;
   bool Success = Data.parse(Content);
   if(!Success){
      *pError = "Could not parse content in file: " + FileName;
      return false;
   }
   if(!Data.isArray()){
      *pError = "Expected content to be an array in file: " + FileName;
      return false;
   }
   for (const auto& it : Data.array()) { Owners.push_back(it); }
   return true;
}

COLstring GITEencodeString(const COLstring& String) {
   COL_FUNCTION(GITEencodeString);
   COLarray<COLstring> Parts;
   COLsplit(&Parts, String, "/");
   for(auto& it : Parts) {
      it = SFIuriEscape(it, false);
   }
   COLstring Output;
   COLjoin(&Output, Parts, "/");
   return Output;
}