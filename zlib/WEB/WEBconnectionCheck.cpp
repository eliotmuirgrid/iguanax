// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBconnectionCheck
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 17 May 2023 - 04:27PM
// ---------------------------------------------------------------------------

#include <WEB/WEBconnectionCheck.h>
#include <WEB/WEBserver.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


// TODO - we'll be able to replace this
void WEBserverCheckConnectionExists(COLuint64 ConnectionId, bool* pConnectionExists, WEBserver* pServer){
   COL_FUNCTION(WEBserverCheckConnectionExists);
   COL_VAR(pServer);
   auto i = pServer->m_Connections.find(ConnectionId);
   *pConnectionExists = !!i;
   COL_VAR2(ConnectionId, *pConnectionExists);
}

void WEBsetConnectionCheckClosure(COLclosure2<COLuint64, bool*>*& pClosure, WEBserver* pServer){
   COL_FUNCTION(WEBsetConnectionCheckClosure);
   COLASSERT(pServer);
   pClosure = COLnewClosure2(&WEBserverCheckConnectionExists, pServer);
   COL_VAR(pClosure);
}

void WEBdeleteConnectionCheckClosure(COLclosure2<COLuint64, bool*>*& pClosure){
   COL_FUNCTION(WEBdeleteConnectionCheckClosure);
   delete pClosure;
}
