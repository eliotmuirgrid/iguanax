//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: BLUAremoveSource
//
// Author: Matthew Sobkowski
//
// Implementation
//---------------------------------------------------------------------------
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAremoveSource.h>
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

static void BLUAsaveCallback(bool Success, COLstring SaveError, CFGconfig Config, COLaddress Address) {
   COL_FUNCTION(BLUAsaveCallback);
   COL_VAR3(Success, SaveError, Config);
   Success ? COLrespondSuccess(Address) : COLrespondError(Address, SaveError);
}

static bool BLUAcheckComponentUnlinkPermission(bool FromTranslator, const COLstring& Username, const COLstring& ComponentId){
   COL_FUNCTION(BLUAcheckComponentUnlinkPermission);
   COLstring Error;
   if (!PRMcheckRoleAccess(FromTranslator, Username, ComponentId, ROLid::EDIT_CONNECTIONS, &Error)){
      return false;
   }
   return true;
}

static void BLUAremoveComponentSourceImpl(const COLstring& Guid, const COLstring& Source, SCKloop* pLoop, CFGmap* pMap, BLUAinstanceManager* pBluaManager,
                                          bool FromTranslator, const COLstring& Username, const COLaddress& Address) {
   COL_FUNCTION(BLUAremoveComponentSourceImpl);
   CFGconfig* pTransNode = CFGmapNode(pMap, Guid);
   CFGconfig* pSourceNode = CFGmapNode(pMap, Source);
   if (!pTransNode)  { return COLrespondError(Address, "No component exists with name or id " + Guid); }
   if (!pSourceNode) { return COLrespondError(Address, "No component exists with name or id " + Source); }
   if (!BLUAcheckComponentUnlinkPermission(FromTranslator, Username, pTransNode->CoreConfig.Guid)){
      COL_TRC("Missing permission on destination component " + pTransNode->CoreConfig.Name);
      return COLrespondError(Address, "You do not have permission on " + pTransNode->CoreConfig.Name + " to unlink it");
   }
   if (!BLUAcheckComponentUnlinkPermission(FromTranslator, Username, pSourceNode->CoreConfig.Guid)){
      COL_TRC("Missing permission on source component " + Source);
      return COLrespondError(Address, "You do not have permission on " + Source + " to unlink it");
   }
   if (pBluaManager->m_Instances.count(pTransNode->CoreConfig.Guid)) {
      return COLrespondError(Address, "Please make sure the destination component \""+Guid+"\" is stopped before making configuration changes.");
   }
   CFGconfigRemoveSource(pSourceNode->CoreConfig.Guid, pTransNode);
   SDBIdeleteQueueConsumer(pSourceNode->CoreConfig.Guid, pTransNode->CoreConfig.Guid);
   CFGremoveDeletedSources(pMap);
   COLvar Data = CFGconfigSerialize(*pTransNode);
   COL_VAR(Data);
   SCKwriteFile(pLoop, DIRcomponentConfig(pTransNode->CoreConfig.Guid), Data.json(1), COLnewClosure2(&BLUAsaveCallback, *pTransNode, Address));
}

// /component/source/delete
void BLUAremoveComponentSource(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(BLUAremoveComponentSource);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   BLUAremoveComponentSourceImpl(COLvarRequiredString(Data, "component"),
                                 COLvarRequiredString(Data, "remove_source"),
                                 pLoop, pMap, pBluaManager, Request.FromTranslator, Request.User, Request.Address);
}
