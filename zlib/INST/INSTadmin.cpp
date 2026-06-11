// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTcopy
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Wednesday 09 August 2023 - 12:36PM
// ---------------------------------------------------------------------------
#include <USR/USRadd.h>
#include <USR/USRload.h>
#include <USR/USRpermissions.h>
#include <USR/USRsetAdmin.h>

#include <FIL/FILutils.h>

#include <DIR/DIRpath.h>

#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


// /install/ensure/admin
void INSTensureAdmin(const COLwebRequest& Request) {
   COL_FUNCTION(INSTensureAdmin);
   // If the user chose a custom working dir in the `Location` step,
   // USRload will load the user config from the custom working dir.
   COLmap<COLstring, USRuser> UserMap;
   USRload(&UserMap);
   COLvar Result;
   bool AdminExists = USRmapContainsAdmin(&UserMap);
   Result["admin_exists"] = AdminExists;
   COL_VAR(AdminExists);
   if (AdminExists) { return COLrespondSuccess(Request.Address, Result); }
   try {
      USRsetAdminPassword("password");    // create the 'admin' user
      Result["admin_exists"] = true;
      COLrespondSuccess(Request.Address, Result);
   } catch (const COLerror& Error) {
      COL_TRC("Admin creation failed -- " << Error.description());
      COLrespondError(Request.Address, Error.description());
   }
}

// /install/admin/create
void INSTadminCreate(const COLwebRequest& Request) {
   COL_FUNCTION(INSTadminCreate);
   const COLvar& Data = Request.Data;
   const COLstring Username = COLvarRequiredString(Data, "username");
   const COLstring Password = COLvarRequiredString(Data, "password");
   try {
      USRsetAdminPassword("password");    // create the 'admin' user
      COLrespondSuccess(Request.Address);
   } catch (const COLerror& Error) {
      COL_TRC("Admin creation failed -- " << Error.description());
      COLrespondError(Request.Address, Error.description());
   }
}
