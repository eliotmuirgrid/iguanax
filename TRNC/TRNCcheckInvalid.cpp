//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCcheckInvalid
//
// Description: Implementation
//
// Author: Matthew Sobkowski
//---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <TRNC/TRNCcheckInvalid.h>
COL_LOG_MODULE;

// /component/check_invalid
void TRNCcheckInvalid(const COLwebRequest& Request, CFGmap* pMap) {
   COLvar Result;
   Result["invalid"] = pMap->HasInvalid;
   pMap->HasInvalid = false;
   COL_VAR(Result);
   COLrespondSuccess(Request.Address, Result);
}