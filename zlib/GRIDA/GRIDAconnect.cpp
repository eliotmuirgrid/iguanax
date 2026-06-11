//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiConnect.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
#include <GRIDA/GRIDAconnect.h>
#include <PRM/PRMcallbacks.h>
COL_LOG_MODULE;

// iguanas/connect
void GRIDAconnect(const COLwebRequest& Request, GRIDnode* pNode) {
   COL_FUNCTION(GRIDAconnect);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_PEERS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_PEERS));
   }
   if(!pNode) { return COLrespondError(Request.Address, "Grid not initialized."); }

   const COLstring IguanaId = COLvarRequiredString(Request.Data, "id");

   const auto& Connections = pNode->getPeers();
   if(Connections.count(IguanaId) > 0 && Connections[IguanaId].pConnection &&
      Connections[IguanaId].pConnection->connected()) {
      return COLrespondError(Request.Address, "Connection is already active.");
   }
   COL_TRC("Connecting to peer at " << IguanaId);
   if(!pNode->connect(IguanaId)) { return COLrespondError(Request.Address, "Failed to initiate connection."); }
   COLrespondSuccess(Request.Address);
}
