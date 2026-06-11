// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDtunnel
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 09:58AM
// ---------------------------------------------------------------------------

#include <DBD/DBDtunnel.h>

#include <FIL/FILutils.h>

#include <TUN/TUNclientLoad.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNclientNetwork.h>
#include <TUN/TUNkeyLoadOrGenerate.h>
#include <TWEB/TWEBapiWeb.h>
#include <TWEB/TWEBclient.h>


#include <WEB/WEBserver.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDtunnelInit(WEBserver* pServer, TUNstateClient* pState, SCKloop* pLoop){
   COL_FUNCTION(DBDtunnelInit);
   pState->m_pLoop = pLoop;
   pState->m_pOnData = COLnewClosure2(&TWEBclient, pState, pServer);
	TUNkeyLoadOrGenerate(&pState->m_MyKey);
   COLstring Error;
   if (!TUNclientPeerLoad(pState, &Error)){
      COL_ERR(Error);  // Figure out how to display the error!
   }
   TUNclientNetworkInit(pState);
   // pServer->m_pWillSpecialHandle = COLnewClosure2(&TWEBapiWillHandle);
   // pServer->m_pSpecialHandler    = COLnewClosure1(&TWEBapiHandle, pState, pServer);
}

void DBDtunnelShutdown(TUNstateClient* pState){
   COL_FUNCTION(DBDtunnelShutdown);
   TUNclientShutdown(pState);
}
