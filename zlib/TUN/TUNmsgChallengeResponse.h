#ifndef __TUN_MSG_CHALLENGE_RESPONSE_H__
#define __TUN_MSG_CHALLENGE_RESPONSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgChallengeResponse
//
// Description:
//
// Challenge response
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 05:26PM
// ---------------------------------------------------------------------------

class TUNstateServer;
class COLstring;
class TUNrequestItem;

void TUNmsgChallengeResponse(TUNrequestItem* pRequest, TUNstateServer* pState);

#endif // end of defensive include
