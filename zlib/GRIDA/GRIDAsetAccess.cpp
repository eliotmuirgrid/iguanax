//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiGrantAccess.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILutils.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDserialize.h>
#include <GRIDA/GRIDAsetAccess.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void GRIDupdateReadAccessCallback() {}

static void GRIDupdateReadAccessOnDisc(const COLstring PeerId, const GRIDpeerInfo PeerInfo,
                                       COLauto<COLclosure0> pCallback, const COLaddress Address) {
   COL_FUNCTION(GRIDupdateReadAccessOnDisc);
   COLstring Err;
   GRIDupdateConfigAdd(PeerId, PeerInfo, Err) ? COLrespondSuccess(Address) : COLrespondError(Address, Err);
}

// /iguanas/access/set
void GRIDAsetAccess(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDAsetAccess);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_PEERS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_PEERS));
   }
   if(!pNode) { return COLrespondError(Request.Address, "Grid not initialized."); }
   const COLstring           IguanaId = COLvarRequiredString(Request.Data, "id");
   const GRIDreadAccessLevel Level    = GRIDreadAccessFromString(COLvarRequiredString(Request.Data, "access_level"));
   const auto&               Peers    = pNode->getPeers();
   const auto                InfoIt   = Peers.find(IguanaId);
   if(InfoIt == Peers.end()) { return COLrespondError(Request.Address, "Peer not found."); }
   pNode->setReadAccess(IguanaId, Level);
   COLauto<COLclosure0> pCallback = COLnewClosure0(&GRIDupdateReadAccessCallback);
   pLoop->threadPool()->scheduleTask( COLnewClosure0(&GRIDupdateReadAccessOnDisc, IguanaId, Peers[InfoIt], pCallback, Request.Address));
}
