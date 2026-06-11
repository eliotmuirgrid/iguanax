// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNrequestId
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 10 August 2023 - 02:15PM
// ---------------------------------------------------------------------------

#include <TUN/TUNrequestId.h>
#include <TUN/TUNstateClient.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLuint64 TUNgetNextRequestId(TUNstateClient* pState){
   COL_FUNCTION(TUNgetNextRequestId);
   return ++pState->m_RequestId;
}

COLuint64 TUNcurrentRequestId(TUNstateClient* pState){
   COL_FUNCTION(TUNcurrentRequestId);
   return pState->m_RequestId;
}
