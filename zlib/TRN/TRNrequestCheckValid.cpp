// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNrequestCheckValid
//
// Description:
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 17 May 2023 - 03:49PM
// ---------------------------------------------------------------------------

#include <TRN/TRNrequestCheckValid.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLclosure2<COLuint64, bool*>* pTRNrequestCheckValid = nullptr;

bool TRNcheckRequestStillValid(COLuint64 ConnectionId){
   COL_FUNCTION(TRNcheckRequestStillValid);
   COL_VAR(ConnectionId);
   if (ConnectionId == 0){
      COL_ERR("Eliot you need a better solution than this!");
      // we don't have a way of telling the peer that a connection has closed.
      return true;
   }
   bool ConnectionExists = false;
   COL_VAR(pTRNrequestCheckValid);
   (pTRNrequestCheckValid)->run(ConnectionId, &ConnectionExists);
   if(!ConnectionExists) { COL_TRC("Connection not found - maybe client has closed the connection."); }
   return ConnectionExists;
}
