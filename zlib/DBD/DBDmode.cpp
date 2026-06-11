//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDmode.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   5/2/23 9:58 AM
//  ---------------------------------------------------------------------------
#include <DBD/DBDmode.h>
#include <DBD/DBDworld.h>
#include <COL/COLweb.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DBDmodeAsString(DBD_MODE Mode){
   COL_FUNCTION(DBDmodeAsString);
   COL_VAR((int) Mode);
   switch(Mode){
   case DBD_MODE::INSTALLER:     return COL_T("startup");
   case DBD_MODE::INITIALIZATON: return COL_T("initialization");
   case DBD_MODE::LOG_UNLOCK:    return COL_T("log_unlock");
   case DBD_MODE::NORMAL:
   default:
      return COL_T("normal");
   }
}

// /mode
void DBDmodeWeb(const COLwebRequest& Request, DBDworld* pWorld){
   COL_FUNCTION(DBDmodeWeb);
   COLvar Result;
   Result["mode"] = DBDmodeAsString(pWorld->Mode);
   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
}