// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCorganizationsSet
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 13 June 2025 - 10:09AM
// ---------------------------------------------------------------------------
#include <GITC/GITCorganizationsSet.h>

#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLvar GITCcleanupOrgsInput(const COLvar& Source){
   COL_FUNCTION(GITCcleanupOrgsInput);
   COLvar OutputAsMap;
   for (int i = 0; i < Source.size(); i++){
      COLstring Key = Source[i].asString();
      if (Key.is_null() || Key.isWhitespace()) { continue; }
      OutputAsMap[Key] = true;
   }
   COLvar OutputNoDuplicates;
   OutputNoDuplicates.setArrayType();
   if (!OutputAsMap.size()) return OutputNoDuplicates;
   for (const auto& i : OutputAsMap.map()){
      OutputNoDuplicates.push_back(i->first);
   }
   return OutputNoDuplicates;
}

bool GITCremoveOrgsFile(const COLstring& FileName, COLstring* pError){
   COL_FUNCTION(GITCremoveOrgsFile);
   try {
      if (FILfileExists(FileName)) { FILremove(FileName); }
   } catch (const COLerror& E) {
      COL_ERR(E.description()); 
      *pError = E.description();
      return false;
   }
   return true;
}

// /git/organizations/set
void GITCorganizationsSet(const COLwebRequest& Request) {
	COL_FUNCTION(GITCorganizationsSet);
   COLstring FileName = DIRconfigUserOrgsFile(Request.User);
   COLstring Error;
   COLvar Out = Request.Data;
   if (!Out.isArray())                       { return COLrespondError(Request.Address, "git/organizations/set expects an array of strings."); }
   if (!GITCremoveOrgsFile(FileName, &Error)){ return COLrespondError(Request.Address, Error); }
   // Read orgs into map and back to array to clean up duplicates. Also skips whitespace keys.
   Out = GITCcleanupOrgsInput(Request.Data);
   if (Out.size() == 0 ){ 
      COL_TRC("After cleanup, nothing to save. Return without writing a new file");
      return COLrespondSuccess(Request.Address, Out); 
   }
   try {
      if (!FILfileExists(FileName)) {
         FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      }
      FILwriteFile(FileName, Out.json(1));
   }
   catch (const COLerror& E) { 
      COL_ERR(E.description()); 
      Error = E.description();
      return COLrespondError(Request.Address, Error);
   }
   return COLrespondSuccess(Request.Address, Out);
}
