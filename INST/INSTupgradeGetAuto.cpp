//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: INSTupgradeGetAuto.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   15/11/23 3:44 PM
//  ---------------------------------------------------------------------------
#include <INST/INSTupgradeGetAuto.h>

#include <DIR/DIRpath.h>

#include <FIL/FILutils.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool INSTupgradeGetAutoImpl(COLvar* pResult, COLstring* pErr) {
   COL_FUNCTION(INSTupgradeGetAutoImpl);
   pResult->setMapType();
   if (!FILfileExists(DIRfileConfigAutoUpgrade())) { return true; }
   try {
      COLstring Data;
      FILreadFile(DIRfileConfigAutoUpgrade(), &Data);
      if (Data.is_null()) { return true; }
      if (!pResult->parse(Data)) {
         *pErr = "Invalid data format in " + DIRfileConfigAutoUpgrade();
         return false;
      }
      return true;
   } catch (const COLerror& E) {
      *pErr = E.description();
      return false;
   }
}

// upgrade/get_auto
void INSTupgradeGetAuto(const COLwebRequest& Request) {
   COL_FUNCTION(INSTupgradeGetAuto);
   COLvar Result;
   COLstring Err;
   bool Success = INSTupgradeGetAutoImpl(&Result, &Err);
   Success ? COLrespondSuccess(Request.Address, Result) : COLrespondError(Request.Address, Err);
}