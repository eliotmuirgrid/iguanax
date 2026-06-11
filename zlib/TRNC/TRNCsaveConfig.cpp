//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCsaveConfig
//
// Implementation.
//
// Author: Matthew Sobkowski
// Date:   Wednesday, June 15, 2022
//---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigSave.h>
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <SCK/SCKworker.h>
#include <TRNC/TRNCsaveConfig.h>
COL_LOG_MODULE;

static void TRNCsaveCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(TRNCsaveCallback);
   COL_VAR(Result);
   Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());
}

static void TRNCsaveConfigWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(TRNCsaveConfigWorker);
   COLstring       Error;
   const COLstring File    = DIRcomponentConfig(In["config"]["guid"].asString());
   const bool      Success = FILsave(File, In["config"].json(1), &Error);
   (*pOut)["success"]      = Success;
   if(!Success) { (*pOut)["error"] = Error; }
   COL_VAR(*pOut);
}

void TRNCsaveConfig(const COLstring& Username, const CFGconfig& Config, SCKloop* pLoop, const COLaddress& Address) {
   COL_FUNCTION(TRNCsaveConfig);
   COLvar Data;
   Data["username"] = Username;
   Data["config"]   = CFGconfigSerialize(Config);
   SCKinvokeWorker(pLoop, TRNCsaveConfigWorker, Data, COLnewClosure1(TRNCsaveCallback, Address));
}
