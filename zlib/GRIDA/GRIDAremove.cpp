//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiRemove.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDserialize.h>
#include <GRIDA/GRIDAremove.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void GRIDupdateConnectionMapCallback() {}

static void GRIDupdateConnectionMap(const COLstring PeerId, COLauto<COLclosure0> pCallback, const COLaddress Address) {
   COL_FUNCTION(GRIDupdateConnectionMap);
   COLstring Error;
   GRIDupdateConfigRemove(PeerId, Error) ? COLrespondSuccess(Address) : COLrespondError(Address, Error);
}

// /iguanas/remove
void GRIDAremovePeer(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDAremovePeer);
   COL_VAR(Request.Data);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_PEERS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_PEERS));
   }
   if(!pNode) { return COLrespondError(Request.Address, "Grid not initialized."); }
   const COLstring IguanaId = COLvarRequiredString(Request.Data, "id");
   pNode->disconnect(IguanaId);
   pNode->removePeer(IguanaId);
   COLauto<COLclosure0> pCallback = COLnewClosure0(&GRIDupdateConnectionMapCallback);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GRIDupdateConnectionMap, IguanaId, pCallback, Request.Address));
}