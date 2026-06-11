// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAoldest
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday 21 September 2023 - 08:51AM
// ---------------------------------------------------------------------------
#include <SDBU/SDBUutils.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SDB/SDBdir.h>
#include <SDBA/SDBAoldest.h>
COL_LOG_MODULE;

// /log/queue/oldest // TODO - this is not needed we have a way of doing this with arguments without creating new apis
void SDBAoldest(const COLwebRequest& Request){
   COL_FUNCTION(SDBAoldest);
   COL_VAR(Request.Data.json(1));
   const COLvar& Data = Request.Data;
   COLstring ComponentId, Error;
   if(!SDBUcomponentId(COLvarGetString(Request.Data, "component", ""), &ComponentId, &Error)){ 
      return COLrespondError(Request.Address, Error); 
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::VIEW_LOGS, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::VIEW_LOGS));
   }
   COLstring OldestFile = SDBfindOldest(SDBdir(ComponentId, SDBroot()));
   COLstring NewestFile = SDBfindNewest(SDBdir(ComponentId, SDBroot()));
   if (OldestFile.is_null() || NewestFile.is_null()) {
      COLrespondError(Request.Address, "The specified component has no log files.");
      return;
   }
   COLvar Result;
   COLuint64 EpochTime = SDBlogFileTime(OldestFile, ComponentId) * 1000;
   Result["oldest"]    = COLuint64ToString(EpochTime);
   EpochTime           = SDBlogFileTime(NewestFile, ComponentId) * 1000;
   Result["newest"]    = COLuint64ToString(EpochTime);
   COL_VAR(Result);
   COLrespondSuccess(Request.Address, Result);
}