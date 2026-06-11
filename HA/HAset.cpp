//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HAset.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <HA/HAset.h>
#include <HA/HAtypes.h>
COL_LOG_MODULE;

void HAset(const COLwebRequest& Request) {
   COL_FUNCTION(HAset);
   const bool      Enabled = COLvarRequiredBool(Request.Data, "enabled");
   const COLstring Dir     = COLvarRequiredString(Request.Data, "shared_working_directory");
   const HAtype    Type    = HAstringToType(COLvarRequiredString(Request.Data, "type"));
   COLvar          Data;
   Data["enabled"]                  = Enabled;
   Data["shared_working_directory"] = Dir;
   Data["type"]                     = HAtypeToString(Type);
   COLstring Err;
   FILsave(DIRfileConfigHa(), Data.json(1), &Err) ? COLrespondSuccess(Request.Address)
                                                  : COLrespondError(Request.Address, Err);
   COL_VAR(Err);
}