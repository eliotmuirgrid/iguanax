// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBApurgeRules
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday 21 September 2023 - 08:51AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <DRX/DRXutils.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKworker.h>
#include <SDB/SDBpurge.h>
#include <SDB/SDBsystem.h>
#include <SDBA/SDBApurgeRules.h>
COL_LOG_MODULE;

// /log/purge_rules
void SDBApurgeRulesGet(const COLwebRequest& Request, SDBsystem* pSystem) {
   COL_FUNCTION(SDBApurgeRulesGet);
   COLvar Result;
   SDBpurgeRulesToVar(pSystem->LogPurgeRules, &Result);
   COLrespondSuccess(Request.Address, Result);
}

static void SDBApurgeRulesSaved(COLvar Result, COLaddress Address) {
   COL_FUNCTION(SDBApurgeRulesSaved);
	COL_VAR(Result);
	Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());

}

static void SDBApurgeRulesSetImpl(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(SDBApurgeRulesSetImpl);
	COLstring Error;
	const bool Success = FILsave(DIRfileConfigLogging(), In["rules"].json(1), &Error);
	(*pOut)["success"] = Success;
	if(!Success) { (*pOut)["error"] = Error; }
}

// /log/purge_rules/set
void SDBApurgeRulesSet(const COLwebRequest& Request, SDBsystem* pSystem, SCKloop* pLoop) {
   COL_FUNCTION(SDBApurgeRulesSet);
	COL_VAR(Request.Data);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::LOG_PURGING, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::LOG_PURGING));
   }
   // TODO - verify input before clearing the rules
   pSystem->LogPurgeRules.clear();
   SDBpurgeRulesFromVar(&pSystem->LogPurgeRules, Request.Data);
   COLvar Data;
	Data["user"] = Request.User;
   SDBpurgeRulesToVar(pSystem->LogPurgeRules, &Data["rules"]);
	SCKinvokeWorker(pLoop, SDBApurgeRulesSetImpl, Data, COLnewClosure1(SDBApurgeRulesSaved, Request.Address));
}

// /log/purge_rules/alt_dir
void SDBApurgeRulesAltDir(const COLwebRequest& Request) {
   COL_FUNCTION(SDBApurgeRulesAltDir);
   COLvar Result;
   Result["alt_enabled"] = DRXhighAvailabilityEnabled();
   COLrespondSuccess(Request.Address, Result);
}