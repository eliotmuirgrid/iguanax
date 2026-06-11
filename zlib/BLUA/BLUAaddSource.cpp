//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: BLUAaddSource
//
// Author: Matthew Sobkowski
//
// Implementation
//---------------------------------------------------------------------------
#include <BLUA/BLUAaddSource.h>
#include <BLUA/BLUAmanager.h>
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static void BLUAsaveCallback(COLvar Result, COLaddress Address) {
   COL_FUNCTION(BLUAsaveCallback);
   COL_VAR(Result);
   Result["success"].asBool()? COLrespondSuccess(Address) : COLrespondError(Address, Result["error"].asString());
}

static void BLUAwriteWorker(const COLvar& In, COLvar* pOut) {
	COL_FUNCTION(BLUAwriteWorker);
	try {
		const COLstring File = DIRcomponentConfig(In["config"]["guid"].asString());
		FILwriteFile(File, In["config"].json(1));
		(*pOut)["success"] = true;
	} catch(const COLerror& E) {
		(*pOut)["success"] = false;
		(*pOut)["error"]	 = E.description();
	}
}

static void BLUAaddComponentSourceImpl(const COLstring& Guid, const COLstring& SourceGuid, SCKloop* pLoop, CFGmap *pMap, BLUAinstanceManager* pBluaManager,
                                       bool FromTranslator, const COLstring& Username, const COLaddress& Address) {
   COL_FUNCTION(BLUAaddComponentSourceImpl);
   COL_VAR2(Guid, SourceGuid);
   COLvar Result;
	CFGconfig* pTransNode  = CFGmapNode(pMap, Guid);
	CFGconfig* pSourceNode = CFGmapNode(pMap, SourceGuid);
   if (!pTransNode)  { return COLrespondError(Address, "No component exists with name or id " + Guid); }
   if (!pSourceNode) { return COLrespondError(Address, "No component exists with name or id " + SourceGuid); }
	COLstring Err;
	PRMcheckRoleAccess(FromTranslator, Username, pTransNode->CoreConfig.Guid, ROLid::EDIT_CONNECTIONS, &Err);
   if (!PRMcheckRoleAccess(FromTranslator, Username, pTransNode->CoreConfig.Guid, ROLid::EDIT_CONNECTIONS, &Err)){
      COL_TRC("Missing permission on destination component " + pTransNode->CoreConfig.Name);
      return COLrespondError(Address, PRMsetError(ROLid::EDIT_CONNECTIONS));
   }
   if (!PRMcheckRoleAccess(FromTranslator, Username, pSourceNode->CoreConfig.Guid, ROLid::EDIT_CONNECTIONS, &Err)){
      COL_TRC("Missing permission on source " + pSourceNode->CoreConfig.Name);
      return COLrespondError(Address, PRMsetError(ROLid::EDIT_CONNECTIONS));
   }
   if (pSourceNode->CoreConfig.Guid == pTransNode->CoreConfig.Guid || pTransNode->CoreConfig.Sources.indexOf(pSourceNode->CoreConfig.Guid) != -1) {
      COL_TRC("Adding self as a source or the source already exists. NOP.");
      return COLrespondSuccess(Address);
   }
   if (pBluaManager->m_Instances.count(pTransNode->CoreConfig.Guid)) {
      return COLrespondError(Address, "Please make sure the destination component \""+Guid+"\" is stopped before making configuration changes.");
   }
   CFGconfigAddSource(pSourceNode->CoreConfig.Guid, pTransNode);
   if (CFGmapHasComponentCycle(pMap, pTransNode->CoreConfig.Guid)) {
      COL_TRC("Cycle detected. The link is not allowed");
      CFGconfigRemoveSource(pSourceNode->CoreConfig.Guid, pTransNode);
      return COLrespondError(Address, "Component cycles are not supported.");
   }
   COL_TRC("Create queue consumer");
   SDBIcreateQueueConsumer(pSourceNode->CoreConfig.Guid, pTransNode->CoreConfig.Guid);
   CFGremoveDeletedSources(pMap);
	COLvar Data;
	Data["user"]	= Username;
	Data["config"] = CFGconfigSerialize(*pTransNode);
	SCKinvokeWorker(pLoop, BLUAwriteWorker, Data, COLnewClosure1(BLUAsaveCallback, Address));
}

// /component/source/add
void BLUAaddComponentSource(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(BLUAaddComponentSource);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   BLUAaddComponentSourceImpl(COLvarRequiredString(Data, "component"),
                             COLvarRequiredString(Data, "source"),
                             pLoop, pMap, pBluaManager, Request.FromTranslator, Request.User, Request.Address);
}
