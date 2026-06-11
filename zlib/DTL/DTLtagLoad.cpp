//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DTLtagLoad.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/03/24 12:27 PM
//  ---------------------------------------------------------------------------
#include <DTL/DTLtagLoad.h>

#include <DIR/DIRpath.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool DTLtagLoadImpl(const COLstring& User, COLvar* pInterfaces, COLstring* pError){
   COL_FUNCTION(DTLtagLoadImpl);
   const COLstring UserInterfaces = DIRconfigUserDir(User) + "filter_tags.json";
   COLvar& Interfaces = *pInterfaces;
   Interfaces.setArrayType();
   if(!FILfileExists(UserInterfaces)) {
      COL_TRC(UserInterfaces << " does not exist");
      return true; // not an error, just means there are no tags defined.
   }
   COLstring RawData;
   try                           { FILreadFile(UserInterfaces, &RawData); }
   catch (const COLerror& Error) { 
      *pError = Error.description();
      return false; 
   }
   if(RawData.is_null()) { return true; }
   if(!Interfaces.parse(RawData) || !Interfaces.isArray()) {
      *pError = "Invalid data";
      return false;
   }
   return true;  
}

void DTLtagLoad(const COLwebRequest& Request) {
   COL_FUNCTION(DTLtagLoad);
   COL_VAR(Request.Data);
   COLvar Interfaces;
   COLstring Error;
   if (!DTLtagLoadImpl(Request.User, &Interfaces, &Error)) { COLrespondError(Request.Address, Error); } 
   else                                                    { COLrespondSuccess(Request.Address, Interfaces); }
}