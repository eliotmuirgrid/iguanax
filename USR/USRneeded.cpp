// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRneeded
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 27 May 2023 - 09:12AM
// ---------------------------------------------------------------------------

#include <USR/USRneeded.h>
#include <USR/USRuser.h>
#include <USR/USRpermissions.h>

#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /user/needed
void USRneeded(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap){
   COL_FUNCTION(USRneeded);
   COLvar Result;
   bool AdminExists = USRmapContainsAdmin(pUserMap);
   COL_VAR(AdminExists);
   Result["new_account_required"] = !AdminExists;
   COLrespondSuccess(Request.Address, Result);
}