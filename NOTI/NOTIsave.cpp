// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTIsave.cpp
//
//  Description: Save notification config
//
//  Author: Matthew Sobkowski
//  Date:   8/31/22, 10:53 AM
// ---------------------------------------------------------------------------
#include <NOTI/NOTIsave.h>
#include <NOTI/NOTImodel.h>
#include <SCK/SCKworker.h>
#include <FIL/FILutils.h>
#include <DIR/DIRpath.h>
#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

static void NOTIconfigSaved(COLvar Result, COLaddress Address) {
   COL_FUNCTION(NOTIconfigSaved);
	Result["success"].asBool() ? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());
}

static void NOTIsaveImpl(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(NOTIsaveImpl);
	COLstring Error;
	const bool Success = FILsave(DIRfileConfigNotifications(), In["config"].json(1), &Error);
	(*pOut)["success"] = Success;
	if(!Success) { (*pOut)["error"] = Error; }
}

// /notifications/set
void NOTIsave(const COLwebRequest& Request,NOTImodel* pNoti, CFGmap* pMap, SCKloop* pLoop) {
	COL_FUNCTION(NOTIsave);
	COLvar RequestData = Request.Data;
	COL_VAR(RequestData);
	COLstring Component = COLvarRequiredString(RequestData,"component_id");
	bool IsOn           = COLvarRequiredBool(RequestData,"is_on");
	COLvar Rules        = COLvarRequiredObject(RequestData, "rules");
	CFGconfig* pTransNode = CFGmapNode(pMap, Component);

	if (!pTransNode){
      return COLrespondError(Request.Address, "No component exists with name or id " + Component);
   } else {
      Component = pTransNode->CoreConfig.Guid;
   }
	COLstring Err;
	 if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, Component, ROLid::EDIT_NOTIFICATIONS, &Err)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_NOTIFICATIONS));
   }

	NOTImodelFromWebVar(pNoti, Component, IsOn, Rules, &Err);
	if(!Err.is_null()) { return COLrespondError(Request.Address, Err); }
	COLvar Data;
	Data["user"] = Request.User;
	NOTImodelToDiscVar(&Data["config"], *pNoti);
	SCKinvokeWorker(pLoop, NOTIsaveImpl, Data, COLnewClosure1(NOTIconfigSaved, Request.Address));
}