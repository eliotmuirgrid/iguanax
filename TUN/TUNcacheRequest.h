#ifndef __TUN_CACHE_REQUEST_H__
#define __TUN_CACHE_REQUEST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNcacheRequest
//
// Description:
//
// A cache of tunnel requests (as binary strings) made which have not yet responded.  
//
// Author: Vismay Shah 
// Date:   Wednesday 02 August 2023 - 03:12PM
// ---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
class COLstring;
class TUNstateClient;

class TUNrequestInProgress{
public:
   TUNrequestInProgress(COLstring* pData, int TimerId);
   ~TUNrequestInProgress();

   int        m_AckTimer;
   COLstring* m_pBinaryRequestData;
};

void TUNcacheRequest(COLstring* pRequest, int AckTimer, const COLstring& ConnectionId, TUNstateClient* pClient);
void TUNunloadRequest(const COLstring& ConnectionId, TUNstateClient* pClient);


#endif // end of defensive include
