//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiMakeRemoteRequest.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DRX/DRXutils.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDrequest.h>
#include <GRID/GRIDwebSocket.h>
#include <GRIDA/GRIDAremoteRequest.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void GRIDremoteRequestTimeout(GRIDnode* pNode, const COLuint64 ConnectionId) {
   COL_FUNCTION(GRIDremoteRequestTimeout);
   auto& ApiConnections = pNode->getApiConnections();
   if(ApiConnections.count(ConnectionId)) {
      COL_TRC("Remote request timed out, cleaning up connection " << ConnectionId);
      delete ApiConnections[ConnectionId].pCallback;
      ApiConnections.removeKey(ConnectionId);
   }
}

static COLuint64 GRIDsendRemoteRequest(GRIDnode* pNode, SCKloop* pLoop, const COLstring& IguanaId, const COLstring& Api, const COLvar& Data,
                                COLclosure1<const COLvar&>* pCallback) {
   COL_FUNCTION(GRIDsendRemoteRequest);

   GRIDwebSocket* pConnection = GRIDfindConnectedPeer(pNode, IguanaId);
   if(!pConnection) {
      COL_TRC("No connection to this peer.");
      return 0;
   }

   const COLuint64 ConnectionId = GRIDgenerateConnectionId(pNode);
   COLvar          RemoteRequest;
   GRIDbuildRemoteRequestJson(RemoteRequest, Api, Data, ConnectionId);

   const int TimerId = pLoop->addTimer(COLnewClosure0(&GRIDremoteRequestTimeout, pNode, ConnectionId), 30000);
   pNode->getApiConnections()[ConnectionId] = GRIDnode::GRIDapiConnection(pCallback, TimerId);
   COL_TRC("Registered callback " << ConnectionId << " for peer " << IguanaId << ", API: " << Api << ", timer: " << TimerId);
   const COLstring JsonRequest = RemoteRequest.json(0);
   COL_TRC("Sending remote request: " << JsonRequest);
   pConnection->sendJson(JsonRequest);
   return ConnectionId;
}

static void GRIDapiDirectResponseCallback(const COLvar& Result, const COLaddress Address) {
   COL_FUNCTION(GRIDapiDirectResponseCallback);
   if(Result.exists("success") && !Result["success"].asBool()) {
      COLrespondError(Address, Result.exists("error") ? Result["error"].asString() : "Remote API call failed");
   } else {
      COLrespondSuccess(Address, Result);
   }
}

// /iguanas/remote_call
void GRIDAremoteRequest(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop) {
   COL_FUNCTION(GRIDAremoteRequest);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_PEERS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_PEERS));
   }
   if(!pNode) { return COLrespondError(Request.Address, "Grid not initialized."); }
   if(!DRXglobalDashboardEnabled()) { return COLrespondError(Request.Address, "This operation is not available under your current license."); }
   const COLstring IguanaId = COLvarRequiredString(Request.Data, "id");
   const COLstring Api      = COLvarRequiredString(Request.Data, "api");
   COLclosure1<const COLvar&>* pCallback    = COLnewClosure1(&GRIDapiDirectResponseCallback, Request.Address);
   const COLuint64             ConnectionId = GRIDsendRemoteRequest(pNode, pLoop, IguanaId, Api, Request.Data, pCallback);

   if(ConnectionId == 0) {
      delete pCallback;
      return COLrespondError(Request.Address, "Failed to send remote request to: " + IguanaId);
   }
}
