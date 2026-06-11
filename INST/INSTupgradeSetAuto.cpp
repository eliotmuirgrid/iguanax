//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: INSTupgradeSetAuto.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   15/11/23 2:59 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <INST/INSTupgradeSetAuto.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static bool INSTupgradeSetAutoImpl(const COLstring& Time, const COLstring& Day, bool Enabled, COLstring* pErr) {
   COL_FUNCTION(INSTupgradeSetAutoImpl);
   COLvar Json;
   Json["time"]    = Time.is_null() ? "" : Time;
   Json["day"]     = Day.is_null() ? "Any" : Day;
   Json["enabled"] = Enabled;
	const bool Success = FILsave(DIRfileConfigAutoUpgrade(), Json.json(1), pErr);
	COL_VAR2(Success, *pErr);
	return Success;
}

// upgrade/set_auto
void INSTupgradeSetAuto(const COLwebRequest& Request) {
   COL_FUNCTION(INSTupgradeSetAuto);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::UPGRADE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::UPGRADE));
   }

   const COLstring Time    = Request.Data["time"];
   const COLstring Day     = Request.Data["day"];
   const bool      Enabled = Request.Data["enabled"].asBool();
   COLstring Err;
	const bool Success = INSTupgradeSetAutoImpl(Time, Day, Enabled, &Err);
   Success ? COLrespondSuccess(Request.Address) : COLrespondError(Request.Address, Err);
}