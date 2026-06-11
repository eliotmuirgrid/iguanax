// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstopListening
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 02 August 2023 - 10:21AM
// ---------------------------------------------------------------------------

#include <TUN/TUNstopListening.h>
#include <TUN/TUNstateClient.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /tunnel/stop
void TUNstopListening(const COLwebRequest& Request, TUNstateClient* pClient){
   COL_FUNCTION(TUNstopListening);
   COL_TRC("Clearing the on data callback - blocking out incoming tunnel data.");
   pClient->m_Respond = false;
   COLrespondSuccess(Request.Address);
}