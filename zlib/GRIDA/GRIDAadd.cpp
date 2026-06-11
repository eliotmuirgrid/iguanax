//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiAdd.cpp
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
#include <GRIDA/GRIDAadd.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void GRIDupdateConnectionMapCallback() {}

static void GRIDupdateConnectionMap(const COLstring PeerId, const GRIDpeerInfo Info, COLauto<COLclosure0> pCallback,
                                    const COLaddress Address) {
   COL_FUNCTION(GRIDupdateConnectionMap);
   COLstring Error;
   GRIDupdateConfigAdd(PeerId, Info, Error) ? COLrespondSuccess(Address) : COLrespondError(Address, Error);
}

// /iguanas/add
void GRIDAaddPeer(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDAaddPeer);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_PEERS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_PEERS));
   }
   if(!pNode) { return COLrespondError(Request.Address, "Grid not initialized."); }

   const COLstring IguanaId    = COLvarRequiredString(Request.Data, "id");
   const COLstring Fingerprint = COLvarRequiredString(Request.Data, "fingerprint");
   const auto      Level       = GRIDreadAccessFromString(COLvarGetString(Request.Data, "access_level", "none"));
   const COLstring Port        = COLvarRequiredString(Request.Data, "port");
   COLstring       Host        = COLvarRequiredString(Request.Data, "host");

   if(Host.find("http://") == 0)  { Host = Host.substr(7); }
   if(Host.find("https://") == 0) { Host = Host.substr(8); }
   COL_VAR4(IguanaId, Fingerprint, Host, Port);

   const GRIDpeerInfo Info = {Host, atoi(Port.c_str()), Fingerprint, Level};
   COLstring          Err;
   if(!pNode->addPeer(IguanaId, Info, Err)) { return COLrespondError(Request.Address, Err); }
   if(!pNode->connect(IguanaId)) { return COLrespondError(Request.Address, "Failed to connect to peer."); }
   COLauto<COLclosure0> pCallback = COLnewClosure0(&GRIDupdateConnectionMapCallback);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GRIDupdateConnectionMap, IguanaId, Info, pCallback, Request.Address));
}
