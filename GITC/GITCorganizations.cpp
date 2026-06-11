// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCorganizations
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 13 June 2025 - 09:53AM
// ---------------------------------------------------------------------------
#include <GITC/GITCorganizations.h>

#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool GITCreadOrgsFile(const COLstring& FileName, COLstring* pContent, COLstring* pError){
   COL_FUNCTION(GITCreadOrgsFile);
   try{
      FILreadFile(FileName, pContent);
      return true;
   } catch (const COLerror& Error){
      *pError = Error.description();
      return false;
   }
}

bool GITCprocessOrganizations(const COLstring& FileName, const COLstring& Content, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITCprocessOrganizations);
   COLvar Data;
   bool Success = Data.parse(Content);
   if(!Success){
      *pError = "Could not parse content in file: " + FileName;
      return false;
   }
   if(!Data.isArray()){
      *pError = "Expected content to be an array in file: " + FileName;
      return false;
   }
   for (const auto& it : Data.array()) { pResult->push_back(it); }
   return true;
}

// similar to GITCorganizations, but used by /git/owners
bool GITCorgsFileContent(const COLstring& Username, COLvar* pContent){
   COL_FUNCTION(GITCorgsFileContent);
   COLstring FileName = DIRconfigUserOrgsFile(Username);
   if (!FILfileExists(FileName)){
      COL_TRC("No organizations file found");
      return false;
   }
   COLstring Content, Error;
   if (!GITCreadOrgsFile(FileName, &Content, &Error)){ 
      COL_TRC("Failed to read organizations file");
      return false; 
   }
   COLvar& Result = *pContent;
   Result.setArrayType();
   if (!GITCprocessOrganizations(FileName, Content, &Result, &Error)){
      COL_TRC("Failed to process content of organizations file");
      return false;
   }
   return (Result.size() > 0);
}

// /git/organizations
void GITCorganizations(const COLwebRequest& Request) {
	COL_FUNCTION(GITCorganizations);
   COLstring FileName = DIRconfigUserOrgsFile(Request.User);
   COLstring Error;
   COLvar Result;
   Result.setArrayType();
	COL_VAR(FileName);
   if (!FILfileExists(FileName)){
      COL_TRC("No organizations file found");
      return COLrespondSuccess(Request.Address, Result); 
   }
   COLstring Content;
   if (!GITCreadOrgsFile(FileName, &Content, &Error)){ 
      COL_TRC("Failed to read organizations file");
      return COLrespondError(Request.Address, Error); 
   }
   if (!GITCprocessOrganizations(FileName, Content, &Result, &Error)){
      return COLrespondError(Request.Address, Error); 
   } else {
      return COLrespondSuccess(Request.Address, Result); 
   }
}